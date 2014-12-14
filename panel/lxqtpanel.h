/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef LXQTPANEL_H
#define LXQTPANEL_H

#include <QSet>

#include <QFrame>
#include <QString>
#include <QTimer>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QAbstractNativeEventFilter>
#endif

#include "ilxqtpanel.h"
#include "lxqtpanelglobals.h"
#include <QtCore/qvariantanimation.h>

class QMenu;
class Plugin;
class VariantAnimation;

namespace LxQt {
class Settings;
class PluginInfo;
}
class LxQtPanelLayout;

/*! \brief The LxQtPanel class provides a single lxqt-panel.
 */
class LXQT_PANEL_API LxQtPanel : public QFrame, public QAbstractNativeEventFilter, public ILxQtPanel
{
    Q_OBJECT

    Q_PROPERTY(QString position READ qssPosition)

public:
    enum Alignment {
        AlignmentLeft   = -1,
        AlignmentCenter =  0,
        AlignmentRight  =  1
    };

    enum AutohideMsg {
        NoMsg = 0,
        RemoveWindow = 1,
        SaveWindow = 2,
        SysTrayConfigure = 3
    };

    LxQtPanel(const QString &configGroup, QWidget *parent = 0);
    virtual ~LxQtPanel();

    QString name() { return mConfigGroup; }

    void readSettings();

    void showPopupMenu(Plugin *plugin = 0);

    // ILxQtPanel .........................
    ILxQtPanel::Position position() const { return mPosition; }
    QRect globalGometry() const;
    QRect calculatePopupWindowPos(const ILxQtPanelPlugin *plugin, const QSize &windowSize) const;

    // For QSS properties ..................
    QString qssPosition() const;

    static bool canPlacedOn(int screenNum, LxQtPanel::Position position);
    static QString positionToStr(ILxQtPanel::Position position);
    static ILxQtPanel::Position strToPosition(const QString &str, ILxQtPanel::Position defaultValue);

    // Settings
    int panelSize() const { return mPanelSize; }
    int iconSize() const { return mIconSize; }
    int lineCount() const { return mLineCount; }
    int length() const { return mLength; }
    bool lengthInPercents() const { return mLengthInPercents; }

    bool autohideTb() const {return mAutoHideTb; }
    int autohideDuration() const {return mAutoHideDuration; }

    LxQtPanel::Alignment alignment() const { return mAlignment; }
    int screenNum() const { return mScreenNum; }
    QColor fontColor() const { return mFontColor; };
    QColor backgroundColor() const { return mBackgroundColor; };
    QString backgroundImage() const { return mBackgroundImage; };
    int opacity() const { return mOpacity; };

    LxQt::Settings *settings() const { return mSettings; }

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *);

public slots:
    void show();

    // Settings

    // autohide
    void setAutohide(bool value);
    void setAutohideDuration(int value);
    void autohidePermanentLock();
    void autohidePermanentUnlock();

    void setPanelSize(int value, bool save);
    void setIconSize(int value, bool save);
    void setLineCount(int value, bool save);
    void setLength(int length, bool inPercents, bool save);
    void setPosition(int screen, ILxQtPanel::Position position, bool save);
    void setAlignment(LxQtPanel::Alignment value, bool save);
    void setFontColor(QColor color, bool save);
    void setBackgroundColor(QColor color, bool save);
    void setBackgroundImage(QString path, bool save);
    void setOpacity(int opacity, bool save);

    void saveSettings(bool later=false);
    void ensureVisible();

signals:
    void realigned();
    void deletedByUser(LxQtPanel *self);

    void pluginAdded(QString id);
    void pluginRemoved(QString id);

protected:
    bool event(QEvent *event);
    void showEvent(QShowEvent *event);
    void childEvent(QChildEvent *event);

private slots:
    void addPlugin(const LxQt::PluginInfo &desktopFile);
    void showConfigDialog();
    void showAddPluginDialog();
    void realign();

    void removePlugin();
    void pluginMoved();
    void userRequestForDeletion();

    //autohide    
    void updateOffset();

private:
    LxQtPanelLayout* mLayout;
    LxQt::Settings *mSettings;
    QFrame *LxQtPanelWidget;
    QString mConfigGroup;
    QList<Plugin*> mPlugins;

    int findAvailableScreen(LxQtPanel::Position position);
    void updateWmStrut();

    void loadPlugins();
    Plugin *loadPlugin(const LxQt::PluginInfo &desktopFile, const QString &settingsGroup);
    Plugin *findPlugin(const ILxQtPanelPlugin *iPlugin) const;

    QString findNewPluginSettingsGroup(const QString &pluginType) const;

    int mPanelSize;
    int mIconSize;
    int mLineCount;

    int mLength;
    bool mLengthInPercents;

    // autohide
    bool mPanelActivated;
    bool mChilds;
    bool mAutoHideTb;
    int  mAutoHideDuration;
    bool mAutoHideActive;
    bool mAutoHideLock;    
    unsigned long int mAutohidePanel;
    int  mAutoHidePermanentLock;    
    void autohideActive(bool value);
    void autohideLock ();
    void autohideCheckLock ();
    void autohideUnlock ();
    QSet<long int> mMapped;
    QRect rect;
    int mOffset;
    int mAnimationOffset;
    VariantAnimation *animationPanel;

    Alignment mAlignment;

    ILxQtPanel::Position mPosition;
    int mScreenNum;
    QTimer mDelaySave;

    QColor mFontColor;
    QColor mBackgroundColor;
    QString mBackgroundImage;
    // 0 to 100
    int mOpacity;

    void updateStyleSheet();
};

class VariantAnimation : public QVariantAnimation
{
  Q_OBJECT

public:
  VariantAnimation(QObject* parent = 0, int* val = 0):
  QVariantAnimation(parent) { currentValue = val; }

private:
  int *currentValue;

signals:
  void valueChanged();

protected:
  virtual void updateCurrentValue(const QVariant& value);
};

#endif // LXQTPANEL_H
