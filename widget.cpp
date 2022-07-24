#include "widget.h"
#include "ui_widget.h"
#include <QIcon>
#include <QMessageBox>

#include <QFile>
#include <QDebug>
#include <QDir>
#include <QTime>

#include <QMediaContent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    setFixedSize(340, 107);

    ui->setupUi(this);

    play_icon = QIcon(":/icons/play.png");
    pause_icon = QIcon(":/icons/pause.png");
    last_icon = QIcon(":/icons/precedent.png");
    next_icon = QIcon(":/icons/suivant.png");
    del_icon = QIcon(":/icons/poubelle.png");
    setWindowIcon(QIcon(":/icons/myMusic.png"));

    repeat_all_icon = QIcon(":/icons/repeat.png");
    no_repeat_icon.addFile("");
    repeat_current_icon = QIcon(":/icons/repeat_current.png");
    random_icon = QIcon(":/icons/random.png");

    ui->pp_btn->setIcon(play_icon);
    ui->last_btn->setIcon(last_icon);
    ui->next_btn->setIcon(next_icon);
    ui->delete_btn->setIcon(del_icon);

    ui->random_btn->setIcon(random_icon);
    ui->reapet_btn->setIcon(repeat_all_icon);

    ui->random_btn->setChecked(false);
    ui->tempsTotal_label->setText("00:00");
    ui->volume_slider->setValue(50);

    player = new QMediaPlayer;
    playlist = new QMediaPlaylist;
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    player->setVolume(50);
    for (auto link : get_liste_musiques())
        playlist->addMedia(QMediaContent(QUrl::fromLocalFile(link.simplified())));

    player->setPlaylist(playlist);
    player->pause();
    update_song_name();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(changeOpacity()));

    connect(ui->pp_btn, SIGNAL(clicked(bool)), this, SLOT(toogle_pp()));    //pause play
    connect(ui->volume_slider, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));    //volume
    connect(playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(update_song_name()));    //nom de la chanson
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(update_playtime_slider())); //son -> slider
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(set_time_readed(qint64)));  //label temps lu
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(set_time_total(qint64)));   //label temps a lire
    connect(ui->playTime_sliderr, SIGNAL(sliderPressed()), player, SLOT(pause())); //slider -> son
    connect(ui->playTime_sliderr, SIGNAL(sliderReleased()), player, SLOT(play())); //slider -> son
    connect(ui->playTime_sliderr, SIGNAL(sliderMoved(int)), this, SLOT(update_song_position(int))); //slider -> son
    connect(ui->playTime_sliderr, SIGNAL(actionTriggered(int)), this, SLOT(update_song_position_by_step(int))); //slider -> pageUP or pageDOWN
}

Widget::~Widget()
{
    delete ui;
}

QStringList Widget::get_liste_musiques()
{
    system("mkdir $HOME/.cache/myMusic 2> /dev/null");
    system("mkdir $HOME/.cache/myMusic/deleted  2> /dev/null");
    system("rm $HOME/patrick/.cache/myMusic/tts.txt  2> /dev/null");
    system("locate $HOME/Musique | grep .mp3$ > $HOME/.cache/myMusic/tts.txt  2> /dev/null");

    QStringList liste;
    QFile fichier(QDir::homePath() + "/.cache/myMusic/tts.txt");

    if (!fichier.open(QIODevice::ReadOnly))
    {
        qDebug() << "Echec d'ouverture du fichier tts.txt";
        return liste;
    }

    while (!fichier.atEnd())
    {
        QString ligne = fichier.readLine();
        liste << ligne;
    }
    return liste;
}

QString Widget::mlsToString(qint64 milli)
{
    return QTime(0, 0).addMSecs(milli).toString("mm:ss");
}

QString Widget::get_current_song_name()
{
    return playlist->currentMedia().canonicalUrl().fileName().replace(".mp3", "");
}

void Widget::toogle_pp()
{
    if (player->state() == QMediaPlayer::PausedState)
    {
        ui->pp_btn->setIcon(pause_icon);
        player->play();
    }
    else if (player->state() == QMediaPlayer::PlayingState)
    {
        ui->pp_btn->setIcon(play_icon);
        player->pause();
    }
}

void Widget::on_next_btn_clicked()
{
    QMediaPlaylist::PlaybackMode current_pbm = playlist->playbackMode();
    if (current_pbm != QMediaPlaylist::Random)
        playlist->setPlaybackMode(QMediaPlaylist::Loop);

    if (playlist->nextIndex() != -1)
        playlist->next();
    else
        playlist->setCurrentIndex(0);

    playlist->setPlaybackMode(current_pbm);
}

void Widget::set_time_readed(qint64 time)
{
    ui->tempsLu_label->setText(mlsToString(time));
}

void Widget::set_time_total(qint64 time)
{
    ui->tempsTotal_label->setText(mlsToString(time));
}

void Widget::update_song_name()
{
    QString song_name = get_current_song_name();
    ui->nomMusique_label->setText(song_name);
    ui->nomMusique_label->setToolTip(song_name);
}

void Widget::update_playtime_slider()
{
    int pos = player->position();
    int dur = player->duration();
    if (player->state() != QMediaPlayer::PausedState)
        ui->playTime_sliderr->setValue(dur <= 0 ? 0 : pos * 100 / dur);
}

void Widget::update_song_position(int valeur)
{
    int dur = ui->playTime_sliderr->value();
    dur = player->duration();
    player->setPosition(valeur * dur / 100);
}

void Widget::update_song_position_by_step(int action)
{
    if (action == QAbstractSlider::SliderPageStepAdd)
        update_song_position(ui->playTime_sliderr->value() + ui->playTime_sliderr->pageStep());
    else if (action == QAbstractSlider::SliderPageStepSub)
        update_song_position(ui->playTime_sliderr->value() - ui->playTime_sliderr->pageStep());
}

void Widget::changeOpacity()
{
    if (qApp->focusWidget() == 0)
        setWindowOpacity(0.3);
    else
        setWindowOpacity(1);
}

void Widget::on_last_btn_clicked()
{
    QMediaPlaylist::PlaybackMode current_pbm = playlist->playbackMode();
    if (current_pbm != QMediaPlaylist::Random)
        playlist->setPlaybackMode(QMediaPlaylist::Loop);

    if (player->position() < 5000)
    {
        if (playlist->previousIndex() != -1)
            playlist->previous();
        else
            playlist->setCurrentIndex(playlist->mediaCount() - 1);
    }
    else
        player->setPosition(0);

    playlist->setPlaybackMode(current_pbm);
}

void Widget::on_delete_btn_clicked()
{
    if (QMessageBox::question(this, "Suppression", "Veux tu vraiment supprimer ?,<br> <b>" + get_current_song_name() + "</b>") == QMessageBox::Yes)
    {
        QString commande("mv '");
        commande.append(playlist->currentMedia().canonicalUrl().toString().remove("file://").append("' $HOME/.cache/myMusic/deleted 2> /dev/null"));
        system(commande.toUtf8().data());
        playlist->removeMedia(playlist->currentIndex());
        update_song_name();
    }
}

void Widget::on_reapet_btn_clicked()
{
    if (playlist->playbackMode() == QMediaPlaylist::Loop)
    {
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->reapet_btn->setIcon(repeat_current_icon);
    }
    else if (playlist->playbackMode() == QMediaPlaylist::CurrentItemInLoop)
    {
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        ui->reapet_btn->setIcon(repeat_all_icon);
    }
}

void Widget::on_random_btn_clicked()
{
    bool checked = ui->random_btn->isChecked();
    if (checked)
    {
        playlist->setPlaybackMode(QMediaPlaylist::Random);
        ui->reapet_btn->setIcon(repeat_all_icon);
    }
    else
    {
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        ui->reapet_btn->setIcon(repeat_all_icon);
    }
}
