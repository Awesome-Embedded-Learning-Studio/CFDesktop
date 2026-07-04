#pragma once

/**
 * @file    cfasciiart.h
 * @brief   Main include file for CFDesktop ASCII Art Library.
 *
 * Provides console-adaptive ASCII art rendering including the CFDesktop
 * logo and decorative star fields.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup desktop_base
 */

#include <string>
#include <vector>

/**
 * @brief Library namespace for CFDesktop ASCII Art
 */
namespace cf::asciiart {

/**
 * @brief Border style enumeration
 */
enum class BorderStyle {
    None,
    Single,  // ┌─┐│
    Double,  // ╔═╗║
    Rounded, // ╭─╮│
    Solid    // █▀
};

/**
 * @brief  Calculates the terminal display width of a UTF-8 string.
 *
 * Correctly handles multi-byte UTF-8 characters and returns the number
 * of terminal columns the string occupies.
 *
 * @param[in]  s        The UTF-8 string to measure.
 * @return               Display width in terminal columns.
 * @throws               None
 * @note                 None
 * @warning              None
 * @since                N/A
 * @ingroup              desktop_base
 */
int display_width(const std::string& s);

/**
 * @brief  Canvas class for ASCII art drawing.
 *
 * Provides a 2D canvas for drawing ASCII art with automatic positioning,
 * centering, and border rendering. Uses a cell-based internal model to
 * correctly handle multi-byte UTF-8 characters.
 *
 * @note           Uses cell-based buffer for multi-byte UTF-8 character support.
 * @warning        None
 * @since          N/A
 * @ingroup        desktop_base
 */
class Canvas {
  public:
    /**
     * @brief  Creates a canvas with specified dimensions.
     *
     * @param[in]  width   Canvas width in characters (display columns).
     * @param[in]  height  Canvas height in characters.
     * @throws             None
     * @note               None
     * @warning            None
     * @since              N/A
     * @ingroup            desktop_base
     */
    Canvas(int width, int height);

    /**
     * @brief  Returns the canvas width in display columns.
     *
     * @return     Canvas width in characters (display columns).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      N/A
     * @ingroup    desktop_base
     */
    int width() const { return width_; }

    /**
     * @brief  Returns the canvas height in rows.
     *
     * @return     Canvas height in characters (rows).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      N/A
     * @ingroup    desktop_base
     */
    int height() const { return height_; }

    /**
     * @brief  Clears the canvas by filling with spaces.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      N/A
     * @ingroup    desktop_base
     */
    void clear();

    /**
     * @brief  Draws text at an absolute position on the canvas.
     *
     * @param[in] x     X coordinate (0-based, from left, in display columns).
     * @param[in] y     Y coordinate (0-based, from top).
     * @param[in] text  Text to draw.
     * @throws          None
     * @note            None
     * @warning         Behavior undefined if position is outside canvas bounds.
     * @since           N/A
     * @ingroup         desktop_base
     */
    void draw(int x, int y, const std::string& text);

    /**
     * @brief  Draws text centered horizontally at the specified row.
     *
     * @param[in] y     Y coordinate for the text line (0-based, from top).
     * @param[in] text  Text to draw.
     * @throws          None
     * @note            None
     * @warning         Behavior undefined if y is outside canvas bounds.
     * @since           N/A
     * @ingroup         desktop_base
     */
    void drawCentered(int y, const std::string& text);

    /**
     * @brief  Draws multi-line text centered both horizontally and vertically.
     *
     * @param[in] text  Multi-line text to draw.
     * @throws          None
     * @note            Newlines in text are treated as line breaks.
     * @warning         None
     * @since           N/A
     * @ingroup         desktop_base
     */
    void drawCentered(const std::string& text);

    /**
     * @brief  Draws a border around the canvas content.
     *
     * @param[in] style  Border style to use.
     * @throws           None
     * @note             Border occupies one cell on each edge.
     * @warning          None
     * @since            N/A
     * @ingroup          desktop_base
     */
    void drawBorder(BorderStyle style);

    /**
     * @brief  Returns the complete rendered canvas as a string.
     *
     * @return     Complete rendered output with newlines between rows.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      N/A
     * @ingroup    desktop_base
     */
    std::string render() const;

    /**
     * @brief  Returns a specific line from the canvas.
     *
     * @param[in] y  Line index (0-based, from top).
     * @return       The line content as a string.
     * @throws       None
     * @note         None
     * @warning      Behavior undefined if y is outside canvas bounds.
     * @since        N/A
     * @ingroup      desktop_base
     */
    std::string getLine(int y) const;

  private:
    int width_;
    int height_;

    // Cell-based buffer: cells_[y][x] contains the string at that position
    // Each cell represents one display column
    std::vector<std::vector<std::string>> cells_;

    void ensureBufferSize();
    static std::vector<std::string> splitLines(const std::string& text);

    // Helper to iterate through UTF-8 string and extract graphemes
    struct Grapheme {
        std::string str;
        int width; // Display width (1 or 2)
    };
    static std::vector<Grapheme> extractGraphemes(const std::string& text);
};

/**
 * @brief Library version information
 */
struct Version {
    static constexpr int MAJOR = 1;
    static constexpr int MINOR = 0;
    static constexpr int PATCH = 0;

    static constexpr const char* toString() { return "1.0.0"; }
};

} // namespace cf::asciiart

// Convenience macros
#define CFASCIIART_VERSION_STRING "1.0.0"
