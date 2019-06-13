#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QPixmap>

class waypoint;
class QPainter;
class MainWindow;
class Tower;
class Gun;
class Music;

class Enemy: public QObject
{
    Q_OBJECT
public:
    Enemy(waypoint *startWayPoint, MainWindow *game, int _difficulty);
    ~Enemy();
    //Enemy(MainWindow *game, int road);

    void draw(QPainter *painter);
    void move();
    void getRemoved();
    void getDamage(int damage);
    void getAttacked(Tower* tower);
    void getLostSight(Tower* tower);
    const QPoint pos();

    int difficulty;
    int road;


protected slots:
    void do_Activate();

private:
    double e_walkspeed;
    int e_currentHP;
    int e_maxHP;
    bool e_active;
    double e_rotationSprite;
    waypoint * e_desWayPoint;//终点

    QPoint e_pos;
    const QPixmap e_sprite1;
    const QPixmap e_sprite2;
    const QPixmap e_sprite3;
    const QPixmap e_sprite4;
    const QPixmap e_sprite5;
    const QPixmap e_sprite9;
    const QPixmap e_sprite10;
    MainWindow * e_game;
    QList<Tower *>	attackedTowerslist;//被攻击的塔列表
//    QList<Gun *> attackedGunlist;

    static const QSize fixedSize;
    Music *music;
};

#endif // ENEMY_H
