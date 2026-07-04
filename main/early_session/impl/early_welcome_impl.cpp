#include "early_welcome_impl.h"
#include "cfasciiart.h"
#include "cflog.h"
#include "device/console/console_helper.h"
#include <QDebug>
#include <any>
#include <iostream>
#include <sstream>

namespace cf::desktop::early_stage {

namespace {
// ANSI 颜色代码
namespace ansi {
constexpr const char* reset = "\033[0m";
constexpr const char* bold = "\033[1m";
constexpr const char* cyan = "\033[96m";
constexpr const char* blue = "\033[94m";
constexpr const char* green = "\033[92m";
constexpr const char* yellow = "\033[93m";
constexpr const char* magenta = "\033[95m";
constexpr const char* white = "\033[97m";
constexpr const char* grey = "\033[90m";
constexpr const char* red = "\033[91m";
} // namespace ansi

// 毒瘤版本号函数
std::string get_version() {
    return "v1.0";
}

// 毒瘤标语函数
std::string get_slogan() {
    return "Welcome! CFDesktop!";
}

// 辅助：剥除 ANSI 转义序列（用于计算纯视觉宽度）
// 必须在所有调用它的函数之前定义。
std::string strip_ansi(const std::string& s) {
    std::string out;
    bool in_esc = false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\033') {
            in_esc = true;
        } else if (in_esc) {
            if (s[i] == 'm')
                in_esc = false;
        } else {
            out += s[i];
        }
    }
    return out;
}

// 对齐辅助函数 - 固定宽度居中
// visual_len 须传入剥除 ANSI 后的视觉宽度，不能用 text.length()。
std::string center_text(const std::string& text, int visual_len, int width) {
    int padding = width - visual_len;
    if (padding <= 0)
        return text;
    int left = padding / 2;
    int right = padding - left;
    return std::string(left, ' ') + text + std::string(right, ' ');
}

// 对齐辅助函数 - 固定宽度居中（带边框，ANSI 安全）
std::string center_in_border(const std::string& text, int inner_width) {
    int visual_len = cf::asciiart::display_width(strip_ansi(text));
    return center_text(text, visual_len, inner_width);
}

// 检测终端是否支持彩色
bool supports_color() {
    cf::base::device::console::ConsoleHelper console;
    std::any value;
    if (console.query_property("colorable", &value)) {
        return std::any_cast<bool>(value);
    }
    return false;
}

// 彩色包装辅助函数
std::string colorize(const std::string& text, const char* ansi_color, bool use_color) {
    return use_color ? std::string(ansi_color) + text + ansi::reset : text;
}

// 辅助：计算多行字符串的行数
int line_count(const std::string& s) {
    if (s.empty())
        return 0;
    int n = 1;
    for (char c : s) {
        if (c == '\n')
            ++n;
    }
    return n;
}

// 辅助：计算多行字符串（已剥除 ANSI）的最大显示宽度
int max_display_width(const std::string& s) {
    std::string clean = strip_ansi(s);
    int max_w = 0;
    std::istringstream iss(clean);
    std::string line;
    while (std::getline(iss, line)) {
        int w = cf::asciiart::display_width(line);
        max_w = std::max(max_w, w);
    }
    return max_w;
}

// ─────────────────────────────────────────────────────────────────────────────
// 用纯 std::vector<std::string> 管理星星背景，完全不依赖 Canvas 的 getLine。
// Canvas 的 cells_ 用 std::string 存储，getLine 的裁剪 / null 比较等内部细节
// 与预期行为存在差异，直接绕开最稳妥。
// ─────────────────────────────────────────────────────────────────────────────

// 在 rows[y][x] 写入一个字符（ASCII），越界时静默忽略。
static void star_put(std::vector<std::string>& rows, int x, int y, char c) {
    if (y < 0 || y >= static_cast<int>(rows.size()))
        return;
    if (x < 0 || x >= static_cast<int>(rows[y].size()))
        return;
    rows[y][static_cast<size_t>(x)] = c;
}

// ─────────────────────────────────────────────────────────────────────────────
// 各尺寸 Monitor 渲染字符串（彩色 / 纯色）
// ─────────────────────────────────────────────────────────────────────────────
std::string render_small_monitor_color() {
    const int inner_width = 16;
    const bool use_color = supports_color();
    std::ostringstream oss;
    oss << "    " << colorize("╭────────────────╮", ansi::cyan, use_color) << "\n";
    oss << "    │" << center_in_border("", inner_width) << "│\n";
    oss << "    │" << center_in_border(colorize("CFDesktop", ansi::bold, use_color), inner_width)
        << "│\n";
    oss << "    │" << center_in_border(colorize(get_version(), ansi::green, use_color), inner_width)
        << "│\n";
    oss << "    │" << center_in_border("", inner_width) << "│\n";
    oss << "    " << colorize("╰────────────────╯", ansi::cyan, use_color) << "\n";
    return oss.str();
}

std::string render_small_monitor() {
    const int inner_width = 16;
    std::ostringstream oss;
    oss << "    ╭────────────────╮\n";
    oss << "    │" << center_in_border("", inner_width) << "│\n";
    oss << "    │" << center_in_border("CFDesktop", inner_width) << "│\n";
    oss << "    │" << center_in_border(get_version(), inner_width) << "│\n";
    oss << "    │" << center_in_border("", inner_width) << "│\n";
    oss << "    ╰────────────────╯\n";
    return oss.str();
}

std::string render_medium_monitor_color() {
    const int inner_width = 22;
    const bool use_color = supports_color();
    std::ostringstream oss;
    oss << "       " << colorize("╭──────────────────────╮", ansi::cyan, use_color) << "\n";
    oss << "       │" << center_in_border("", inner_width) << "│\n";
    oss << "       │" << center_in_border(colorize("CFDesktop", ansi::bold, use_color), inner_width)
        << "│\n";
    oss << "       │"
        << center_in_border(colorize(get_version(), ansi::green, use_color), inner_width) << "│\n";
    oss << "       │" << center_in_border("", inner_width) << "│\n";
    oss << "       │"
        << center_in_border(colorize(get_slogan(), ansi::magenta, use_color), inner_width) << "│\n";
    oss << "       │" << center_in_border("", inner_width) << "│\n";
    oss << "       " << colorize("╰──────────────────────╯", ansi::cyan, use_color) << "\n";
    return oss.str();
}

std::string render_medium_monitor() {
    const int inner_width = 22;
    std::ostringstream oss;
    oss << "       ╭──────────────────────╮\n";
    oss << "       │" << center_in_border("", inner_width) << "│\n";
    oss << "       │" << center_in_border("CFDesktop", inner_width) << "│\n";
    oss << "       │" << center_in_border(get_version(), inner_width) << "│\n";
    oss << "       │" << center_in_border("", inner_width) << "│\n";
    oss << "       │" << center_in_border(get_slogan(), inner_width) << "│\n";
    oss << "       │" << center_in_border("", inner_width) << "│\n";
    oss << "       ╰──────────────────────╯\n";

    return oss.str();
}

std::string render_large_monitor_color() {
    constexpr int inner_width = 34;
    bool use_color = supports_color();
    std::ostringstream oss;
    std::string ib = colorize("│", ansi::cyan, use_color);
    oss << "         " << colorize("╭──────────────────────────────────╮", ansi::cyan, use_color)
        << "\n";
    oss << "         │" << center_in_border("", inner_width) << "│\n";
    oss << "         │"
        << center_in_border(colorize("CFDesktop", ansi::bold, use_color), inner_width) << "│\n";
    oss << "         │"
        << center_in_border(colorize(get_version(), ansi::green, use_color), inner_width) << "│\n";
    oss << "         │" << center_in_border("", inner_width) << "│\n";
    oss << "         │  " << colorize("┌────────────────────────────┐", ansi::cyan, use_color)
        << "  │\n";
    oss << "         │  " << ib << "  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄  " << ib << "  │\n";
    oss << "         │  " << ib << " █    " << colorize("Code Editor", ansi::blue, use_color)
        << " ▄▄▄     █ " << ib << "  │\n";
    oss << "         │  " << ib << " █▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█ " << ib << "  │\n";
    oss << "         │  " << ib << "  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄  " << ib << "  │\n";
    oss << "         │  " << ib << " █   " << colorize("Terminal", ansi::green, use_color)
        << "    ▄▄▄▄▄▄▄▄▄█ " << ib << "  │\n";
    oss << "         │  " << ib << " █▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█ " << ib << "  │\n";
    oss << "         │  " << colorize("└────────────────────────────┘", ansi::cyan, use_color)
        << "  │\n";
    oss << "         │"
        << center_in_border(colorize(get_slogan(), ansi::magenta, use_color), inner_width) << "│\n";
    oss << "         │" << center_in_border("", inner_width) << "│\n";
    oss << "         " << colorize("╰──────────────────────────────────╯", ansi::cyan, use_color);
    return oss.str();
}

std::string render_large_monitor() {
    constexpr int inner_width = 34;
    std::ostringstream oss;
    oss << "         ╭──────────────────────────────────╮\n";
    oss << "         │" << center_in_border("", inner_width) << "│\n";
    oss << "         │" << center_in_border("CFDesktop", inner_width) << "│\n";
    oss << "         │" << center_in_border(get_version(), inner_width) << "│\n";
    oss << "         │" << center_in_border("", inner_width) << "│\n";
    oss << "         │  ┌────────────────────────────┐  │\n";
    oss << "         │  │  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄  │  │\n";
    oss << "         │  │ █    Code Editor ▄▄▄     █ │  │\n";
    oss << "         │  │ █▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█ │  │\n";
    oss << "         │  │  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄  │  │\n";
    oss << "         │  │ █   Terminal    ▄▄▄▄▄▄▄▄▄█ │  │\n";
    oss << "         │  │ █▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█ │  │\n";
    oss << "         │  └────────────────────────────┘  │\n";
    oss << "         │" << center_in_border(get_slogan(), inner_width) << "│\n";
    oss << "         │" << center_in_border("", inner_width) << "│\n";
    oss << "         ╰──────────────────────────────────╯";
    return oss.str();
}

// ─── 替换原来的 make_star_rows ───────────────────────────────────────────────
// 返回 {字符行, 颜色矩阵}，颜色矩阵 nullptr 表示该格无星星 / 不着色。
std::pair<std::vector<std::string>, std::vector<std::vector<const char*>>>
make_star_rows(int canvas_width, int canvas_height, int logo_col_start, int logo_col_end,
               int logo_row_start, int logo_row_end, int density, bool use_color) {

    std::vector<std::string> rows(static_cast<size_t>(canvas_height),
                                  std::string(static_cast<size_t>(canvas_width), ' '));

    // 颜色矩阵：nullptr = 空格/无色
    std::vector<std::vector<const char*>> color_grid(
        static_cast<size_t>(canvas_height),
        std::vector<const char*>(static_cast<size_t>(canvas_width), nullptr));

    const char star_chars[] = {'*', '+', '.', '`', '*', '.', '+', '*'};
    const int num_types = static_cast<int>(sizeof(star_chars));

    // 星星可用的颜色池
    const char* star_palette[] = {ansi::cyan,    ansi::blue,  ansi::green, ansi::yellow,
                                  ansi::magenta, ansi::white, ansi::grey};
    const int num_colors = static_cast<int>(sizeof(star_palette) / sizeof(star_palette[0]));

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto place = [&](int x, int y) {
        if (x < 0 || x >= canvas_width || y < 0 || y >= canvas_height)
            return;
        rows[static_cast<size_t>(y)][static_cast<size_t>(x)] = star_chars[std::rand() % num_types];
        if (use_color)
            color_grid[static_cast<size_t>(y)][static_cast<size_t>(x)] =
                star_palette[std::rand() % num_colors];
    };

    // ── 计算四个区块的面积，按比例分配星星数量 ──────────────────────────

    // 区块 A：左侧列，全高（x ∈ [0, logo_col_start)，y ∈ [0, canvas_height)）
    int area_left = logo_col_start * canvas_height;

    // 区块 B：右侧列，全高（x ∈ (logo_col_end, canvas_width)，y ∈ [0, canvas_height)）
    int area_right = std::max(0, canvas_width - logo_col_end) * canvas_height;

    // 区块 C：上方中列（x ∈ [logo_col_start, logo_col_end)，y ∈ [0, logo_row_start)）
    int mid_cols = std::max(0, logo_col_end - logo_col_start);
    int area_top = mid_cols * std::max(0, logo_row_start);

    // 区块 D：下方中列（x ∈ [logo_col_start, logo_col_end)，y ∈ (logo_row_end, canvas_height)）
    int area_bot = mid_cols * std::max(0, canvas_height - logo_row_end);

    auto scatter = [&](int area, auto x_rand, auto y_rand) {
        int num = area * density / 100;
        for (int i = 0; i < num; ++i)
            place(x_rand(), y_rand());
    };

    // A: 左侧
    if (area_left > 0)
        scatter(
            area_left, [&] { return std::rand() % logo_col_start; },
            [&] { return std::rand() % canvas_height; });

    // B: 右侧
    if (area_right > 0)
        scatter(
            area_right, [&] { return logo_col_end + std::rand() % (canvas_width - logo_col_end); },
            [&] { return std::rand() % canvas_height; });

    // C: 上方
    if (area_top > 0)
        scatter(
            area_top, [&] { return logo_col_start + std::rand() % mid_cols; },
            [&] { return std::rand() % logo_row_start; });

    // D: 下方
    if (area_bot > 0)
        scatter(
            area_bot, [&] { return logo_col_start + std::rand() % mid_cols; },
            [&] { return logo_row_end + std::rand() % (canvas_height - logo_row_end); });

    return {std::move(rows), std::move(color_grid)};
}

// ─── 替换原来的 render_with_stars ────────────────────────────────────────────
std::string render_with_stars(int canvas_width, int canvas_height, const std::string& monitor,
                              int density, cf::asciiart::BorderStyle border,
                              bool use_color) { // ← 新增参数
    int logo_lines = line_count(monitor);
    int logo_width = max_display_width(monitor);

    int logo_col_start = std::max(0, (canvas_width - logo_width) / 2);
    int logo_col_end = logo_col_start + logo_width;

    int logo_row_start = (canvas_height - logo_lines) / 2;
    int logo_row_end = logo_row_start + logo_lines;

    auto [star_rows, color_grid] =
        make_star_rows(canvas_width, canvas_height, logo_col_start, logo_col_end, logo_row_start,
                       logo_row_end, density, use_color);

    std::vector<std::string> logo_line_vec;
    {
        std::istringstream iss(monitor);
        std::string ln;
        while (std::getline(iss, ln))
            logo_line_vec.push_back(ln);
    }

    int start_row = logo_row_start;

    std::string result;
    for (int row = 0; row < canvas_height; ++row) {
        const std::string& srow = star_rows[static_cast<size_t>(row)];
        int logo_idx = row - start_row;

        // ── 构建左侧星星片段（带颜色）──────────────────────────────────────
        auto colored_segment = [&](int from_col, int to_col) -> std::string {
            std::string seg;
            int len = static_cast<int>(srow.size());
            for (int c = from_col; c < to_col; ++c) {
                char ch = (c < len) ? srow[static_cast<size_t>(c)] : ' ';
                if (use_color && ch != ' ') {
                    const char* col = color_grid[static_cast<size_t>(row)][static_cast<size_t>(c)];
                    if (col) {
                        seg += col;
                        seg += ch;
                        seg += ansi::reset;
                        continue;
                    }
                }
                seg += ch;
            }
            return seg;
        };

        if (logo_idx >= 0 && logo_idx < static_cast<int>(logo_line_vec.size())) {
            const std::string& logo_ln = logo_line_vec[static_cast<size_t>(logo_idx)];
            int vw = cf::asciiart::display_width(strip_ansi(logo_ln));
            std::string padded_logo = logo_ln + std::string(std::max(0, logo_width - vw), ' ');

            std::string left = colored_segment(0, logo_col_start);
            std::string right = colored_segment(logo_col_end, canvas_width);

            // 裁掉右侧尾部空格（ANSI 安全：尾部空格不含转义）
            size_t rend = right.rfind('\033');
            if (rend == std::string::npos) {
                size_t re2 = right.find_last_not_of(' ');
                right = (re2 == std::string::npos) ? "" : right.substr(0, re2 + 1);
            }
            // 若末尾是 reset 后全空格，也清掉
            result += left + padded_logo + right + "\n";
        } else {
            std::string sline = colored_segment(0, canvas_width);
            // 裁掉尾部空格（无色时简单处理，有色时保留 reset 前内容）
            if (!use_color) {
                size_t send = sline.find_last_not_of(' ');
                sline = (send == std::string::npos) ? "" : sline.substr(0, send + 1);
            }
            result += sline + "\n";
        }
    }

    // ── 拼完所有行之后，按需包边框 ──────────────────────────────────────
    if (border == cf::asciiart::BorderStyle::None)
        return result;

    // 计算视觉宽度（取最宽行）
    int box_inner = canvas_width;

    std::string hbar;
    hbar.reserve(static_cast<size_t>(box_inner) * 3); // UTF-8 每字符最多3字节
    for (int i = 0; i < box_inner; ++i)
        hbar += "─";
    std::string tl = "┌", tr = "┐", bl = "└", br = "┘", vbar = "│";

    if (use_color) {
        tl = colorize(tl, ansi::cyan, true);
        tr = colorize(tr, ansi::cyan, true);
        bl = colorize(bl, ansi::cyan, true);
        br = colorize(br, ansi::cyan, true);
        vbar = colorize(vbar, ansi::cyan, true);
        hbar = colorize(hbar, ansi::cyan, true);
    }

    std::string bordered;
    bordered += tl + hbar + tr + "\n";
    {
        std::istringstream iss(result);
        std::string ln;
        while (std::getline(iss, ln)) {
            int vw = cf::asciiart::display_width(strip_ansi(ln));
            ln += std::string(std::max(0, box_inner - vw), ' '); // 右侧补空格对齐
            bordered += vbar + ln + vbar + "\n";
        }
    }
    bordered += bl + hbar + br;
    return bordered;
}

// ─── 三个入口透传 use_color ──────────────────────────────────────────────────
std::string render_small(int w, int h) {
    const bool use_color = supports_color();
    const std::string monitor = use_color ? render_small_monitor_color() : render_small_monitor();
    int safe_h = std::max(h, line_count(monitor) + 4);
    return render_with_stars(w - 2, safe_h, monitor, 30, cf::asciiart::BorderStyle::None,
                             use_color);
}

std::string render_medium(int w, int h) {
    const bool use_color = supports_color();
    const std::string monitor = use_color ? render_medium_monitor_color() : render_medium_monitor();
    int safe_h = std::max(h, line_count(monitor) + 4);
    return render_with_stars(w - 2, safe_h, monitor, 35, cf::asciiart::BorderStyle::Single,
                             use_color);
}

std::string render_large(int w, int h) {
    const bool use_color = supports_color();
    const std::string monitor = use_color ? render_large_monitor_color() : render_large_monitor();
    int safe_h = std::max(h, line_count(monitor) + 4);
    return render_with_stars(w - 2, safe_h, monitor, 40, cf::asciiart::BorderStyle::Single,
                             use_color);
}

} // namespace

EarlyWelcomeImpl::BootResult EarlyWelcomeImpl::run_session() {
    auto welcome = factorize_bootWelcomeWithString();
    std::cout << welcome << std::endl;
    return BootResult::OK;
}

std::string EarlyWelcomeImpl::factorize_bootWelcomeWithString() const {
    using namespace cf::log;
    auto console_query = base::device::console::ConsoleHelper();
    auto console_size = console_query.size();
    if (!console_size) {
        console_size = console_query.fallback_size();
    }

    int width = console_size->first;
    int height = console_size->second;
    debugftag("BootWelcome", "detected console size: {} x {}", width, height);
    if (width < 60) {
        debug("Runs small shown policy");
        return render_small(width, height);
    } else if (width < 100) {
        debug("Runs medium shown policy");
        return render_medium(width, height);
    } else {
        debug("Runs large shown policy");
        return render_large(width, height);
    }
}

} // namespace cf::desktop::early_stage
