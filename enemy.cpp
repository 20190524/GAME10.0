#include "enemy.h"
#include "waypoint.h"
#include "tower.h"
#include "utility.h"
#include "mainwindow.h"
#include "gun.h"
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QMatrix>
#include <QVector2D>
#include <QtMath>
#include<QMediaPlayer>
#include"music.h"
static const int Health_Bar_Width = 60;
static const int award[]={100,100,150,200,0,0,0,0,400,600};

const QSize Enemy::fixedSize(140,140);

Enemy::Enemy(waypoint *startWayPoint, MainWindow *game, int _difficulty):
        QObject(0)
      , e_active(false)
      , e_rotationSprite(0.0)
      , e_pos(startWayPoint->pos())
      , e_desWayPoint(startWayPoint->nextWayPoint())
      , e_game(game)
      , difficulty(_difficulty)
      , e_sprite1(QPixmap(":/pictures/enemy02.png"))
      , e_sprite2(QPixmap(":/pictures/enemy01.png"))
      , e_sprite3(QPixmap(":/pictures/enemy08.png"))
      , e_sprite4(QPixmap(":/pictures/enemy05.png"))
      , e_sprite5(QPixmap(":/pictures/enemy09.png"))
      , e_sprite9(QPixmap(":/pictures/boss2.png"))
      , e_sprite10(QPixmap(":/pictures/boss1.png"))
{
    if (difficulty==1) {
        e_maxHP=40;
        e_walkspeed=1.0;
    }
    else if (difficulty==2) {
        e_maxHP=60;
        e_walkspeed=1.0;
    }
    else if (difficulty==3) {
        e_maxHP=80;
        e_walkspeed=1.0;
    }
    else if (difficulty==4) {
        e_maxHP=100;
        e_walkspeed=2.0;
    }
    else if (difficulty==5) {
        e_maxHP=120;
        e_walkspeed=2.0;
    }
    else if (difficulty==9) {
        e_maxHP=200;
        e_walkspeed=2.0;
    }
    else if (difficulty==10) {
        e_maxHP=300;
        e_walkspeed=2.0;//为了调试改了
    }
    e_currentHP=e_maxHP;
}



void Enemy::draw(QPainter *painter){
    if (!e_active)
        return;

    painter->save();

    /*QPoint healthBarPoint = e_pos + QPoint(fixedSize.width()/50, -fixedSize.height()/7);
    // 绘制血条
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red);
    QRect healthBarBackRect(healthBarPoint, QSize(Health_Bar_Width, 5));
    painter->drawRect(healthBarBackRect);

    painter->setBrush(Qt::green);
    healthBarPoint = healthBarPoint + QPoint((double)e_currentHP / e_maxHP * Health_Bar_Width, 0);
    QRect healthBarRect(healthBarPoint, QSize((double)(e_maxHP-e_currentHP) / e_maxHP * Health_Bar_Width, 5));
    painter->drawRect(healthBarRect);*/

    // 绘制偏转坐标,由中心+偏移=左上
 //   static const QPoint offsetPoint(-fixedSize.width() / 2, -fixedSize.height() / 2);
    static const QPoint offsetPoint(-fixedSize.width() * 0, -fixedSize.height() * 0);
    painter->translate(e_pos);
    // 绘制敌人
    if (difficulty==1) painter->drawPixmap(offsetPoint, e_sprite1);
    else if (difficulty==2) painter->drawPixmap(offsetPoint, e_sprite2);
    else if (difficulty==3) painter->drawPixmap(offsetPoint, e_sprite3);
    else if (difficulty==4) painter->drawPixmap(offsetPoint, e_sprite4);
    else if (difficulty==5) painter->drawPixmap(offsetPoint, e_sprite5);
    else if (difficulty==9) painter->drawPixmap(offsetPoint, e_sprite9);
    else if (difficulty==10) painter->drawPixmap(offsetPoint, e_sprite10);

    painter->restore();
}


void Enemy::move(){
    if (!e_active)
        return;

    if (collisionWithCircle(e_pos, 1, e_desWayPoint->pos()))
    {
        // 敌人抵达了一个航点
        if (e_desWayPoint->nextWayPoint())
        {
            // 还有下一个航点
            e_pos = e_desWayPoint->pos();
            e_desWayPoint = e_desWayPoint->nextWayPoint();
        }
        else
        {
            // 表示进入基地
            e_game->getHpDamage();
            e_game->removeEnemy(this);
            return;
        }
    }

    // 还在前往航点的路上
    // 目标航点的坐标
    QPoint targetPoint = e_desWayPoint->pos();
    // 未来修改这个可以添加移动状态,加快,减慢,m_walkingSpeed是基准值

    // 向量标准化
    qreal movementSpeed = e_walkspeed;
    QVector2D normalized(targetPoint - e_pos);
    normalized.normalize();
    e_pos = e_pos + normalized.toPoint() * movementSpeed;

    // 确定敌人选择方向
    // 默认图片向左,需要修正180度转右
    e_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x())) + 180;
}

void Enemy::do_Activate(){
    e_active=true;
}

void Enemy::getRemoved() {
    if (attackedTowerslist.empty()) return;
    foreach (Tower * attacker, attackedTowerslist) {
        attacker->enemyKilled();
    }
    music->playMusic(dead);
    /*QMediaPlayer * player = new QMediaPlayer;
    player->setMedia(QUrl("qrc:/sound/dead.mp3"));
    player->setVolume(30);
    player->play();*/

//    music->playMusic(dead);
    e_game->removeEnemy(this);
}


void Enemy::getDamage(int damage) {
    e_currentHP -= damage;
    if (e_currentHP<=0) {
        e_game->awardGold(award[difficulty-1]);
        getRemoved();
    }
}

void Enemy::getAttacked(Tower *attacker) {
    attackedTowerslist.push_back(attacker);
}

void Enemy::getLostSight(Tower *attacker) {
    attackedTowerslist.removeOne(attacker);
}

const QPoint Enemy::pos() {
    return e_pos;
}

Enemy::~Enemy () {
    attackedTowerslist.clear();
    e_desWayPoint = NULL;
    e_game = NULL;
}
