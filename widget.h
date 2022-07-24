#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QIcon>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void toogle_pp();

private slots:
    void on_next_btn_clicked();
    void set_time_readed(qint64 time);
    void set_time_total(qint64 time);
    void update_song_name();
    void update_playtime_slider();
    void update_song_position(int pos);
    void update_song_position_by_step(int action);
    void changeOpacity();

    void on_last_btn_clicked();

    void on_delete_btn_clicked();

    void on_reapet_btn_clicked();

    void on_random_btn_clicked();

private:
    Ui::Widget *ui;
    QIcon pause_icon, play_icon, last_icon, next_icon, del_icon;
    QIcon repeat_all_icon, no_repeat_icon, repeat_current_icon;
    QIcon random_icon;
    QMediaPlayer *player;
    QMediaPlaylist *playlist;

    QStringList get_liste_musiques();
    QString mlsToString(qint64 milli);
    QString get_current_song_name();
};
#endif // WIDGET_H
