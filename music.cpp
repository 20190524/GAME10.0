#include"music.h"
#include<QDir>
#include<QMediaPlayer>
#include<QMediaPlaylist>

static const QString s_curDir="qrc:/";
Music::Music(QObject *parent):QObject(parent){

}

void Music::playMusic(MusicType musictype){
    static const QUrl mediaUrls[]={
        QUrl(s_curDir+"sound/gameover.mp3"),
        QUrl(s_curDir+"sound/dead.mp3"),
        QUrl(s_curDir+"sound/loadtower.mp3"),
        QUrl(s_curDir+"sound/shoot.mp3"),
        QUrl(s_curDir+"sound/updatetower.mp3"),
    };
    static QMediaPlayer player;
    player.setMedia(mediaUrls[musictype]);
    player.setVolume(30);
    player.play();
}
