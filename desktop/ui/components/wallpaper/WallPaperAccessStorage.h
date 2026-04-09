/**
 * @file    desktop/ui/components/wallpaper/WallPaperAccessStorage.h
 * @brief   Indexed storage for wallpaper tokens.
 *
 * Provides an indexed vector of WallPaperToken instances with cursor
 * navigation and overflow signaling.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-04-09
 * @version 0.1
 * @since   0.15
 * @ingroup wallpaper
 */

#pragma once
#include "WallPaperToken.h"
#include "base/indexed_vector/indexed_vector.hpp"
#include "base/weak_ptr/weak_ptr.h"
#include <QImage>
#include <QObject>
#include <memory>

namespace cf::desktop::wallpaper {
/**
 * @brief  Indexed storage for wallpaper tokens with cursor navigation.
 *
 * Wraps an indexed_vector of unique WallPaperToken instances and provides
 * sequential/random access with overflow signaling via Qt signals.
 *
 * @note   None.
 *
 * @ingroup wallpaper
 *
 * @code
 * WallPaperAccessStorage storage;
 * storage.addToken(std::move(token));
 * storage.toNext(true);
 * @endcode
 */
class WallPaperAccessStorage : public QObject {
    Q_OBJECT
  public:
    explicit WallPaperAccessStorage(QObject* parent = nullptr) : QObject(parent) {}
    /**
     * @brief move the index to the next one
     *
     * @return WeakPtr<WallPaperToken>
     */
    WeakPtr<WallPaperToken> toNext(bool move_cursor = false);
    WeakPtr<WallPaperToken> toPrev(bool move_cursor = false);
    WeakPtr<WallPaperToken> to(const wallpaper_token_id_t& token);
    WeakPtr<WallPaperToken> at(const wallpaper_token_id_t& token);

    /**
     * @brief Returns the token at the current cursor position.
     *
     * @return WeakPtr to current token, or null if storage is empty.
     */
    WeakPtr<WallPaperToken> current() const;

    /**
     * @brief  Appends a wallpaper token to the storage.
     *
     * Ownership of the token transfers to this storage.
     *
     * @param[in] token  The wallpaper token to add.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          wallpaper
     */
    void addToken(std::unique_ptr<WallPaperToken> token);

    /**
     * @brief  Inserts a wallpaper token at the given index.
     *
     * Ownership of the token transfers to this storage.
     *
     * @param[in] index  The position at which to insert.
     * @param[in] token  The wallpaper token to insert.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          wallpaper
     */
    void insertToken(size_t index, std::unique_ptr<WallPaperToken> token);

    /**
     * @brief  Returns the index of the token with the given ID.
     *
     * @param[in] token  The token ID to look up.
     *
     * @return           Index of the matching token in the storage.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          wallpaper
     */
    size_t indexOf(const wallpaper_token_id_t& token) const;

    enum class OverFlowType { OverFlow, UnderFlow };
  signals:
    /**
     * @brief  Emitted when cursor navigation exceeds storage bounds.
     *
     * @param[in] t  The type of overflow (over- or under-flow).
     *
     * @throws       None.
     *
     * @note         None.
     * @warning      None.
     * @since        0.15
     * @ingroup      wallpaper
     */
    void indexed_overflow(const OverFlowType t);

  private:
    cf::indexed_vector<std::unique_ptr<WallPaperToken>> wallpaper_images;
};
} // namespace cf::desktop::wallpaper
