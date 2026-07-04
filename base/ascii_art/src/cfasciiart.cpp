#include "cfasciiart.h"
#include <algorithm>
#include <cstdint>
#include <sstream>

namespace cf::asciiart {

namespace {

// Calculate display width of a single UTF-8 codepoint
int codepointWidth(uint32_t cp) {
    // Emoji and Symbols range (includes 🚀 U+1F680)
    if (cp >= 0x1F300 && cp <= 0x1FAFF)
        return 2;
    // Miscellaneous Symbols and Pictographs / Supplemental Symbols
    if (cp >= 0x1F900 && cp <= 0x1F9FF)
        return 2;
    // CJK unified ideographs
    if (cp >= 0x2E80 && cp <= 0xA4CF)
        return 2;
    // Hangul syllables
    if (cp >= 0xAC00 && cp <= 0xD7AF)
        return 2;
    // CJK extensions
    if (cp >= 0xF900 && cp <= 0xFAFF)
        return 2;
    // Fullwidth forms
    if (cp >= 0xFF00 && cp <= 0xFF60)
        return 2;
    // CJK compatibility ideographs
    if (cp >= 0xFE10 && cp <= 0xFE1F)
        return 2;
    if (cp >= 0xFE30 && cp <= 0xFE6F)
        return 2;
    // Ambiguous width characters - treat as wide for safety
    if (cp >= 0x1100 && cp <= 0x115F)
        return 2;
    // Combining characters and zero-width joiners - they modify the previous character
    if (cp == 0x200D || (cp >= 0x20D0 && cp <= 0x20FF))
        return 0;
    // Variation selectors
    if (cp >= 0xFE00 && cp <= 0xFE0F)
        return 0;
    return 1;
}

// Decode a UTF-8 sequence starting at position i
struct Utf8Char {
    uint32_t codepoint;
    size_t byteCount;
};

Utf8Char decodeUtf8(const std::string& s, size_t i) {
    uint32_t cp = 0;
    unsigned char c = s[i];
    size_t bytes = 1;

    if (c < 0x80) {
        cp = c;
        bytes = 1;
    } else if ((c & 0xE0) == 0xC0) {
        cp = c & 0x1F;
        bytes = 2;
    } else if ((c & 0xF0) == 0xE0) {
        cp = c & 0x0F;
        bytes = 3;
    } else if ((c & 0xF8) == 0xF0) {
        cp = c & 0x07;
        bytes = 4;
    } else {
        // Invalid UTF-8, treat as single char
        return {0xFFFD, 1};
    }

    // Check we have enough bytes
    if (i + bytes > s.size()) {
        return {0xFFFD, 1};
    }

    // Decode continuation bytes
    for (size_t j = 1; j < bytes; ++j) {
        unsigned char next = s[i + j];
        if ((next & 0xC0) != 0x80) {
            // Invalid continuation
            return {0xFFFD, 1};
        }
        cp = (cp << 6) | (next & 0x3F);
    }

    return {cp, bytes};
}

} // anonymous namespace

int display_width(const std::string& s) {
    int width = 0;
    size_t i = 0;
    while (i < s.size()) {
        auto utf8 = decodeUtf8(s, i);
        width += codepointWidth(utf8.codepoint);
        i += utf8.byteCount;
    }
    return width;
}

std::vector<Canvas::Grapheme> Canvas::extractGraphemes(const std::string& text) {
    std::vector<Grapheme> graphemes;
    size_t i = 0;

    while (i < text.size()) {
        auto utf8 = decodeUtf8(text, i);
        Grapheme g;
        g.str = text.substr(i, utf8.byteCount);
        g.width = codepointWidth(utf8.codepoint);
        graphemes.push_back(g);
        i += utf8.byteCount;
    }

    return graphemes;
}

Canvas::Canvas(int width, int height) : width_(width), height_(height) {
    ensureBufferSize();
    clear();
}

void Canvas::clear() {
    for (auto& row : cells_) {
        for (auto& cell : row) {
            cell = " ";
        }
    }
}

void Canvas::draw(int x, int y, const std::string& text) {
    if (y < 0 || y >= height_)
        return;

    auto graphemes = extractGraphemes(text);

    // Handle negative x: skip leading graphemes
    size_t graphemeStart = 0;
    if (x < 0) {
        int skipCols = -x;
        for (size_t i = 0; i < graphemes.size(); ++i) {
            skipCols -= graphemes[i].width;
            if (skipCols <= 0) {
                graphemeStart = i + 1;
                x = 0; // Start drawing from column 0
                break;
            }
        }
        if (skipCols > 0)
            return; // All content skipped
    }

    if (x >= width_)
        return;

    int col = x;
    for (size_t i = graphemeStart; i < graphemes.size(); ++i) {
        const auto& g = graphemes[i];
        if (col >= width_)
            break;
        if (col + g.width > width_)
            break; // Would overflow

        cells_[y][col] = g.str;
        if (g.width == 2 && col + 1 < width_) {
            // Mark adjacent cell as occupied (use special marker that render knows to skip)
            cells_[y][col + 1] = "\x00"; // Null marker for continuation
        }
        col += g.width;
    }
}

void Canvas::drawCentered(int y, const std::string& text) {
    if (y < 0 || y >= height_)
        return;
    int textWidth = display_width(text);
    int x = std::max(0, (width_ - textWidth) / 2);
    draw(x, y, text);
}

void Canvas::drawCentered(const std::string& text) {
    auto lines = splitLines(text);
    int startRow = std::max(0, (height_ - static_cast<int>(lines.size())) / 2);

    for (size_t i = 0; i < lines.size(); ++i) {
        int row = startRow + static_cast<int>(i);
        if (row < height_) {
            drawCentered(row, lines[i]);
        }
    }
}

void Canvas::drawBorder(BorderStyle style) {
    if (style == BorderStyle::None)
        return;

    std::string topLeft, topRight, bottomLeft, bottomRight;
    std::string horizontal, vertical;

    switch (style) {
        case BorderStyle::Single:
            topLeft = "┌";
            topRight = "┐";
            bottomLeft = "└";
            bottomRight = "┘";
            horizontal = "─";
            vertical = "│";
            break;
        case BorderStyle::Double:
            topLeft = "╔";
            topRight = "╗";
            bottomLeft = "╚";
            bottomRight = "╝";
            horizontal = "═";
            vertical = "║";
            break;
        case BorderStyle::Rounded:
            topLeft = "╭";
            topRight = "╮";
            bottomLeft = "╰";
            bottomRight = "╯";
            horizontal = "─";
            vertical = "│";
            break;
        case BorderStyle::Solid:
            topLeft = "█";
            topRight = "█";
            bottomLeft = "█";
            bottomRight = "█";
            horizontal = "▀";
            vertical = "█";
            break;
        default:
            return;
    }

    if (width_ < 2 || height_ < 2)
        return;

    // Top border
    cells_[0][0] = topLeft;
    for (int x = 1; x < width_ - 1; ++x) {
        cells_[0][x] = horizontal;
    }
    cells_[0][width_ - 1] = topRight;

    // Bottom border
    cells_[height_ - 1][0] = bottomLeft;
    for (int x = 1; x < width_ - 1; ++x) {
        cells_[height_ - 1][x] = horizontal;
    }
    cells_[height_ - 1][width_ - 1] = bottomRight;

    // Side borders
    for (int y = 1; y < height_ - 1; ++y) {
        cells_[y][0] = vertical;
        cells_[y][width_ - 1] = vertical;
    }
}

std::string Canvas::render() const {
    std::string result;
    for (const auto& row : cells_) {
        std::string line;
        for (const auto& cell : row) {
            // Skip null markers (used for wide character continuation)
            if (cell == "\x00")
                continue;
            line += cell;
        }
        // Trim trailing whitespace
        size_t end = line.find_last_not_of(" \t");
        if (end != std::string::npos) {
            line = line.substr(0, end + 1);
        }
        result += line + "\n";
    }
    return result;
}

std::string Canvas::getLine(int y) const {
    if (y < 0 || y >= height_)
        return "";
    std::string line;
    for (const auto& cell : cells_[y]) {
        // Skip null markers (used for wide character continuation)
        if (cell == "\x00")
            continue;
        line += cell;
    }
    // Trim trailing whitespace
    size_t end = line.find_last_not_of(" \t");
    if (end != std::string::npos) {
        line = line.substr(0, end + 1);
    }
    return line;
}

void Canvas::ensureBufferSize() {
    cells_.resize(height_);
    for (auto& row : cells_) {
        row.resize(width_, " ");
    }
}

std::vector<std::string> Canvas::splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    return lines;
}

} // namespace cf::asciiart
