#ifndef MUSIC_H
#define MUSIC_H

#include<QObject>
class QMediaPlayer;

enum MusicType{
    gameover,
    dead,
    Towerplace,
    shoot,
    Upgrade
};
class Music:public QObject
{
public:
    explicit Music(QObject*parent=0);
    void playMusic(MusicType musictype);
    Music() {}
};
#endif // MUSIC_H
