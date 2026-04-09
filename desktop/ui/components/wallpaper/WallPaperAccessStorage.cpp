#include "WallPaperAccessStorage.h"
#include <algorithm>
#include <stdexcept>

namespace cf::desktop::wallpaper {

WeakPtr<WallPaperToken> WallPaperAccessStorage::toNext(bool move_cursor) {
    try {
        if (move_cursor)
            wallpaper_images.next();
        return wallpaper_images.at(wallpaper_images.cursor() + (move_cursor ? 0 : 1))->getWeakPtr();
    } catch (const std::out_of_range&) {
        emit indexed_overflow(OverFlowType::OverFlow);
        return {};
    }
}

WeakPtr<WallPaperToken> WallPaperAccessStorage::toPrev(bool move_cursor) {
    try {
        if (move_cursor)
            wallpaper_images.prev();
        return wallpaper_images.at(wallpaper_images.cursor() - (move_cursor ? 0 : 1))->getWeakPtr();
    } catch (const std::out_of_range&) {
        emit indexed_overflow(OverFlowType::UnderFlow);
        return {};
    }
}

WeakPtr<WallPaperToken> WallPaperAccessStorage::to(const wallpaper_token_id_t& token) {
    auto it = std::find_if(wallpaper_images.begin(), wallpaper_images.end(),
                           [&token](const auto& ptr) { return ptr->id() == token; });
    if (it == wallpaper_images.end())
        return {};
    wallpaper_images.set_cursor(static_cast<size_t>(it - wallpaper_images.begin()));
    return (*it)->getWeakPtr();
}

WeakPtr<WallPaperToken> WallPaperAccessStorage::at(const wallpaper_token_id_t& token) {
    auto it = std::find_if(wallpaper_images.begin(), wallpaper_images.end(),
                           [&token](const auto& ptr) { return ptr->id() == token; });
    return it != wallpaper_images.end() ? (*it)->getWeakPtr() : WeakPtr<WallPaperToken>{};
}

WeakPtr<WallPaperToken> WallPaperAccessStorage::current() const {
    if (wallpaper_images.empty()) {
        return {};
    }
    return wallpaper_images.at_cursor()->getWeakPtr();
}

void WallPaperAccessStorage::addToken(std::unique_ptr<WallPaperToken> token) {
    wallpaper_images.push_back(std::move(token));
}

void WallPaperAccessStorage::insertToken(size_t index, std::unique_ptr<WallPaperToken> token) {
    wallpaper_images.insert(wallpaper_images.begin() + static_cast<std::ptrdiff_t>(index),
                            std::move(token));
}

size_t WallPaperAccessStorage::indexOf(const wallpaper_token_id_t& token) const {
    auto it = std::find_if(wallpaper_images.begin(), wallpaper_images.end(),
                           [&token](const auto& ptr) { return ptr->id() == token; });
    if (it == wallpaper_images.end())
        throw std::out_of_range("WallPaperAccessStorage::indexOf: token not found");
    return static_cast<size_t>(it - wallpaper_images.begin());
}

} // namespace cf::desktop::wallpaper
