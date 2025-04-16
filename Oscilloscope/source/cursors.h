#ifndef CURSORS_H
#define CURSORS_H

#include <QObject>
#include <vector>
#include <QPen>

struct MouseEvent
{
// todo: use qt version
#if QT_VERSION >= 0x050000
    enum  ev { Released = (Qt::MaxMouseButton << 1), DoubleClicked = (Qt::MaxMouseButton << 2) };
#else
    enum  ev { Released = 0x100, DoubleClicked = 0x200 };
#endif
};

class Cursors : public QObject
{
    Q_OBJECT
public:
    enum eCursor
    {
        noCursor=-1,
        vertical_1,
        vertical_2,
        verticalZoom,
        horizontal_1,
        horizontal_2,
        horizontalZoom,
        CursorsSize
    };

    enum eView
    {
        unknown_view = -1,
        scope_view,
        fft_view,
        zoom_view,
        lissayous_view,
        ViewSize
    };

    explicit Cursors(QObject *parent = 0);
    void     setCursor(eCursor aCursor, double aValue, bool aUpdate=true);
    double   getCursor(eCursor aCursor) const;

    void     setVerticalVisible(bool aVisible);
    bool     isVerticalVisible() const;
    void     setHorizontalVisible(bool aVisible);
    bool     isHorizontalVisible() const;

    void     setID(Cursors::eView aID);
    Cursors::eView getID() const;

    static QPen&    getPen();


Q_SIGNALS:
    void     update(Cursors::eCursor aCursor, Cursors::eView aID);

private:
    bool            mVerticalVisible;
    bool            mHorizontalVisible;
    Cursors::eView  mID;
    std::vector<double> mCursor;
    static QPen     mPen;
};

#endif // CURSORS_H
