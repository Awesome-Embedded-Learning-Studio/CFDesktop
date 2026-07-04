#include "WallPaperToken.h"
#include "aex/weak_ptr/weak_ptr_factory.h"
#include <QStringList>
#include <QUuid>

namespace cf::desktop::wallpaper {

// ============================================================
// WallPaperToken::Private
// aex::WeakPtrFactory placed LAST (destroyed first, invalidates
// weak refs before other members).
// ============================================================
struct WallPaperToken::Private {
    const wallpaper_token_id_t id{QUuid::createUuid().toString(QUuid::WithoutBraces)};

    QString source_path;
    QString display_name;
    QString author; // empty by default; getter returns "Unknown"
    QString description;
    SourceType source_type;

    aex::WeakPtrFactory<WallPaperToken> weak_ptr_factory;

    Private(const QString& path, SourceType type, const QString& name, const QString& auth,
            const QString& desc, WallPaperToken* owner)
        : source_path(path), display_name(name),
          author(auth.isNull() ? QStringLiteral("Unknown") : auth), description(desc),
          source_type(type), weak_ptr_factory(owner) {}
};

// ============================================================
// WallPaperToken
// ============================================================

WallPaperToken::WallPaperToken(const QString& source_path, SourceType type,
                               const QString& display_name, const QString& author,
                               const QString& description)
    : d(std::make_unique<Private>(source_path, type, display_name, author, description, this)) {}

WallPaperToken::~WallPaperToken() = default;

const wallpaper_token_id_t& WallPaperToken::id() const {
    return d->id;
}
const QString& WallPaperToken::sourcePath() const {
    return d->source_path;
}
const QString& WallPaperToken::displayName() const {
    return d->display_name;
}
const QString& WallPaperToken::author() const {
    return d->author;
}
const QString& WallPaperToken::description() const {
    return d->description;
}
auto WallPaperToken::sourceType() const -> SourceType {
    return d->source_type;
}

bool operator==(const WallPaperToken& lh, const WallPaperToken& rh) {
    return lh.d->id == rh.d->id;
}

aex::WeakPtr<WallPaperToken> WallPaperToken::getWeakPtr() const {
    return d->weak_ptr_factory.GetWeakPtr();
}

WallPaperTokenFactory::WallPaperToken_t WallPaperTokenFactory::fromFile(const QString& path) {
    return std::unique_ptr<WallPaperToken>(
        new WallPaperToken(path, WallPaperToken::SourceType::File));
}

std::vector<WallPaperTokenFactory::WallPaperToken_t>
WallPaperTokenFactory::fromFiles(const QStringList& paths) {
    std::vector<WallPaperTokenFactory::WallPaperToken_t> v;
    for (const auto& p : paths) {
        v.emplace_back(std::move(fromFile(p)));
    }
    return v;
}

} // namespace cf::desktop::wallpaper
