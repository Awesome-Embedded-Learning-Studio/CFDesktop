/**
 * @file    desktop/ui/components/wallpaper/WallPaperToken.h
 * @brief   Represents a single wallpaper source.
 *
 * A WallPaperToken holds metadata (path, display name, author, description)
 * and a source type (File or Generated) for a wallpaper image.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-04-09
 * @version 0.1
 * @since   0.15
 * @ingroup wallpaper
 */

#pragma once
#include "base/factory/smartptr_plain_factory.hpp"
#include "base/weak_ptr/weak_ptr.h"
#include <QString>
#include <memory>

namespace cf::desktop::wallpaper {

class WallPaperTokenFactory;

using wallpaper_token_id_t = QString;

/**
 * @brief  Represents a single wallpaper source with metadata.
 *
 * Stores the source path, display name, author, description, and type
 * for a wallpaper. Tokens are created via WallPaperTokenFactory.
 *
 * @note   None.
 *
 * @ingroup wallpaper
 *
 * @code
 * auto factory = WallPaperTokenFactory::fromFile("/path/to/img.png");
 * auto token = factory.create();
 * @endcode
 */
class WallPaperToken {
  public:
    /**
     * @brief  Indicates how the wallpaper image is sourced.
     * @ingroup wallpaper
     */
    enum class SourceType {
        File,      ///< Loaded from a file on disk.
        Generated, ///< Generated programmatically.
    };

    ~WallPaperToken();

    /**
     * @brief  Returns the unique identifier of this token.
     *
     * @return         Const reference to the token ID string.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.15
     * @ingroup        wallpaper
     */
    const wallpaper_token_id_t& id() const;

    /**
     * @brief  Returns the filesystem path of the wallpaper source.
     *
     * @return         Const reference to the source path string.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.15
     * @ingroup        wallpaper
     */
    const QString& sourcePath() const;

    /**
     * @brief  Returns the display name of this wallpaper.
     *
     * @return         Const reference to the display name string.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.15
     * @ingroup        wallpaper
     */
    const QString& displayName() const;

    /**
     * @brief  Returns the author of this wallpaper.
     *
     * @return         Const reference to the author string.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.15
     * @ingroup        wallpaper
     */
    const QString& author() const;

    /**
     * @brief  Returns the description of this wallpaper.
     *
     * @return         Const reference to the description string.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.15
     * @ingroup        wallpaper
     */
    const QString& description() const;

    /**
     * @brief  Returns the source type of this wallpaper.
     *
     * @return         The SourceType enum value.
     *
     * @throws         None.
     *
     * @note           None.
     * @warning        None.
     * @since          0.15
     * @ingroup        wallpaper
     */
    SourceType sourceType() const;
    WeakPtr<WallPaperToken> getWeakPtr() const;

    friend bool operator==(const WallPaperToken& lh, const WallPaperToken& rh);

  protected:
    friend class WallPaperTokenFactory;

    /**
     * @brief  Constructs a WallPaperToken with the given metadata.
     *
     * @param[in] source_path   Path to the wallpaper image source.
     * @param[in] type          How the image is sourced.
     * @param[in] display_name  Human-readable name (default empty).
     * @param[in] author        Author name (default empty).
     * @param[in] description   Description text (default empty).
     *
     * @throws                  None.
     *
     * @note                    None.
     * @warning                 None.
     * @since                   0.15
     * @ingroup                 wallpaper
     */
    explicit WallPaperToken(const QString& source_path, SourceType type,
                            const QString& display_name = {}, const QString& author = {},
                            const QString& description = {});

  private:
    struct Private;
    std::unique_ptr<Private> d;
};

class WallPaperTokenFactory {
  public:
    static WallPaperTokenFactory& fromFile(const QString& path);
    std::unique_ptr<WallPaperToken> create();

  private:
    QString stored_path_;
    WallPaperToken::SourceType stored_type_{WallPaperToken::SourceType::File};
};

} // namespace cf::desktop::wallpaper
