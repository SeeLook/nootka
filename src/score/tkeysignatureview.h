#ifndef TKEYSIGNATUREVIEW_H
#define TKEYSIGNATUREVIEW_H

#include <QGraphicsView>
#include "tscorewidgetsimple.h"


class TscoreWidgetSimple;
class QGraphicsScene;
class QGraphicsSimpleTextItem;

/** User can change key signature by mouse wheel moving, and TKeySignView generated keySignWasChanged
* signal with @p char keySign information.
@author Tomasz Bojczuk  <tomaszbojczuk@gmail.com>
* @short This class represents the field between clef and notes with key signature acidentals.
*/
class TkeySignatureView : public QGraphicsView
{
    friend class TscoreWidgetSimple;

    Q_OBJECT
public:

          /** Constructs TKeySignView class instance
          * @param _keySign The key signature (default is 0 - C-maj)
          * @param parent its parent (have to be TscoreWidget class)
          *        */
    explicit TkeySignatureView(TscoreWidgetSimple *parent = 0, char _keySign = 0);
    ~TkeySignatureView();

    char keySignature() { return m_keySignature; }
            /** Keeps two values of char with data of all major key signatures.
            * majorKeySignatures[0] has values for Ces-maj (1,-1) 1 is C note
            * and -1 is flat. */
    static char majorKeySignatures[15][2];
            /** The same like above, but for minor keys */
    static char minorKeySignatures[15][2];
            /** Arrays with string of names of major and minor keys.*/
    static QString majorKeysNames[15];
    static QString minorKeysNames[15];
            /** It sets fuses to block mouse wheel event and stop adding more flats or sharps*/
    void setKeysFuse(char flatFuse, char sharpFuse);



signals:
        /** Is called to inform @class TscoreWidgetSimple */
    void keySignWasChanged();

public slots:

protected:
    void wheelEvent(QWheelEvent *event);
            /** This metods get and set the key signature, and are called
            * only from their parent @class TscoreWidgetSimple as continuation
            * his public methods */
    void setKeySignature(char keySign);
             /** @class TscoreWidgetSimple as a parent has to take care about resizing process
             * so it calls this function in his resizeEvent and put @param coeff parameter */
    void resize(int co);
        /** setNotation() method prepeares two arrays of QString with names of
        * key signatures (major and minor) in user preferes notation */
    void setNameStyle();



private:
    QGraphicsScene *m_scene;
        /** Array of text items with # or b signs*/
    QGraphicsSimpleTextItem *m_accidentals[7];
    QGraphicsSimpleTextItem *m_keyNameText;

    int m_coeff;
    char m_keySignature;
        /** It keeps array of accidental symbol (# or b) positions
        * (in PosY coordinates from TnoteView)
        * @li [0] is position for f# and fb
        * @li [1] c# and
        * @li etc....    */
    char m_posOfAccid[7];
                /** This points at array @param TscoreWidget::accInKeyArr[7],
                    * which keeps information
                    * about adequate accidental in key signature.
                    * F.e. @p keyAcidsArr[0] is -1 for C# in D-maj key*/
    char *m_accInKeyPtr;
            /** To calculate correct positnion of accidental symbol on the staff */
    int m_accTextOffset;
    char m_flatKeyFuse, m_sharpKeyFuse;

};

#endif // TKEYSIGNATUREVIEW_H
