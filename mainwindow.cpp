#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include<QMediaPlayer>
#include<QMediaPlaylist>
using namespace std;

static const int Towercost[]={100,200,200,300,300};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , waves(1)
  , playerHp(20)
  , playerGold(2000)
  , checkpoint(1)
  , win(false)
  , lose(false)
  ,_startgame(false)
  ,_mainpage(true)
  ,_statement(false)
  ,_close(false)
{
    ui->setupUi(this);
    loadTowerPositions();
    addwaypoint();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
    timer->start(30);
    QTimer::singleShot(3000, this, SLOT(gameStart()));

    /*QMediaPlayer * player = new QMediaPlayer;
    player->setMedia(QUrl("qrc:/sound/musicback.mp3"));
    player->setVolume(30);
    player->play();*/

        QMediaPlaylist *playlist=new QMediaPlaylist;
        playlist->addMedia(QUrl("qrc:/sound/musicback.mp3"));
        //    playlist->
        //    playlist->setCurrentIndex(0);
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        QMediaPlayer *player= new QMediaPlayer;
        player->setPlaylist(playlist);
        player->setVolume(30);
        player->play();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter painter(this);

    if(_mainpage)
    {
        painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/pictures/mainpage.jpg"));
    }

    if(_statement)
    {
        painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/pictures/statement.jpg"));
    }

    if(_startgame)

    {
        painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/pictures/grass.jpg"));

        foreach (const towerposition &towerPos, t_towerpositionlist) {
            towerPos.drawTower(&painter);
        }
        foreach (Tower *tower,t_towerlist) {
            tower->drawTower(&painter);
        }

        /*if (win) {
        painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(""));
    }*/
        //绘制通关胜利的界面，点击界面后游戏结束




        foreach (Enemy*enemy1, enemylist1) enemy1->draw(&painter);
        foreach (Enemy*enemy2, enemylist2) enemy2->draw(&painter);
        foreach (Enemy*enemy3, enemylist3) enemy3->draw(&painter);
        foreach (Enemy*enemy4, enemylist4) enemy4->draw(&painter);

        while (!Bonuses.empty())
        {
            delete Bonuses[0];
            Bonuses.removeAt(0);
        }

        //foreach (const waypoint *wayPoint, waypointlist)
        //wayPoint->draw(&painter);

        foreach (const Bullet *bullet, bulletlist) bullet->drawBullet(&painter);

        drawWave(&painter);
        drawHP(&painter);
        drawPlayerGold(&painter);
        //drawCheckPoint(&painter);
    }
    if(win){
        painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/pictures/youwin.jpg"));
    }
    if(lose)
    {
        painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/pictures/youlose.jpg"));
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if(lose){
        if(event->button()==Qt::LeftButton){
            QApplication* app;
            app->exit(0);
        }
    }
    if(_mainpage)
        {
            if(event->button()==Qt::LeftButton)
            {
                QPoint pressPos = event->pos();
                if(pressPos.rx()<(this->width()/2+100)&&pressPos.rx()>(this->width()/2-100)&&pressPos.ry()<(this->height()/2+70)&&pressPos.ry()>(this->height()/2+20)){
                    _mainpage=false;
                    _statement=true;
                }
                if(pressPos.rx()<(this->width()/2+100)&&pressPos.rx()>(this->width()/2-100)&&pressPos.ry()<(this->height()/2+150)&&pressPos.ry()>(this->height()/2+100)){
                    _mainpage=false;
                    _close=true;
    //                exit(1);
//                    return;//这个没法直接退出了！
                    QApplication* app;
                    app->exit(0);
                }
                if(pressPos.rx()<(this->width()/2+100)&&pressPos.rx()>(this->width()/2-100)&&pressPos.ry()<(this->height()/2+230)&&pressPos.ry()>(this->height()/2+180)){
                    _mainpage=false;
                    _startgame=true;
                }
            }
        }

    if(_statement)
    {
           if(event->button()==Qt::LeftButton)
           {
               QPoint pressPos = event->pos();
               if(pressPos.rx()<this->width()-280&&pressPos.ry()<360&&pressPos.rx()>this->width()-370&&pressPos.ry()>280){
                   _statement=false;
                   _mainpage=true;
               }
           }
    }

    if(_startgame)
    {
        if(event->button()==Qt::LeftButton)
        {
            QPoint pressPos = event->pos();
            if (checkpoint==1) {
                auto it = t_towerpositionlist.begin();
                while (it != t_towerpositionlist.end())
                {
                    if (it->containTower(pressPos) && it->canhaveTower())
                    {
                        if (canbuyTower(1)) {
                            //m_audioPlayer->playSound(TowerPlaceSound);
                            playerGold -= Towercost[0];
                            Tower *tower = new Tower(it->centerPos(), this, 1);
                            it->sethaveTower(tower);
                            t_towerlist.push_back(tower);
                            update();
                            break;
                        }
                        else {
                            QMessageBox box2(QMessageBox::NoIcon, "提示", "金币不足！");
                            box2.setIconPixmap(QPixmap(":/pictures/remind.png"));
                            box2.exec();
                        }
                    }
                    else if (it->containTower(pressPos) && !it->canhaveTower()) {
                        QMessageBox box1(QMessageBox::NoIcon, "提示","是否升级炮塔?", QMessageBox::Yes | QMessageBox::No, NULL);
                        box1.setIconPixmap(QPixmap(":/pictures/gold.png"));
                        if(box1.exec()==QMessageBox::Yes) {
                            if ( it->getTower()->level==1 ) {
                                if (canbuyTower(2)) {
                                    Tower *tower=it->getTower()->upgrade();
                                    removeTower(it->getTower());
                                    it->sethaveTower(tower);
                                    playerGold-=Towercost[1];
                                }
                                else {
                                    QMessageBox box2(QMessageBox::NoIcon, "提示", "金币不足！");
                                    box2.setIconPixmap(QPixmap(":/pictures/remind.png"));
                                    box2.exec();
                                }
                            }
                            else if (it->getTower()->level==2) {
                                if (canbuyTower(3)) {
                                    Tower *tower=it->getTower()->upgrade();
                                    removeTower(it->getTower());
                                    it->sethaveTower(tower);
                                    playerGold-=Towercost[2];
                                }
                                else {
                                    QMessageBox box2(QMessageBox::NoIcon, "提示", "金币不足！");
                                    box2.setIconPixmap(QPixmap(":/pictures/remind.png"));
                                    box2.exec();
                                }
                            }
                            else if (it->getTower()->level==3) {
                                QMessageBox box2(QMessageBox::NoIcon, "提示", "已升至当前最高级炮塔！");
                                box2.setIconPixmap(QPixmap(":/pictures/remind.png"));
                                box2.exec();
                            }
                        }
                    }

                    ++it;
                }
            }
            else if (checkpoint==2) {
                auto it = t_towerpositionlist.begin();
                while (it != t_towerpositionlist.end())
                {
                    if (canbuyTower(1) && it->containTower(pressPos) && it->canhaveTower())
                    {
                        //m_audioPlayer->playSound(TowerPlaceSound);
                        playerGold -= Towercost[0];
                        Tower *tower = new Tower(it->centerPos(), this, 1);
                        it->sethaveTower(tower);
                        t_towerlist.push_back(tower);
                        update();
                        break;
                    }
                    else if (it->containTower(pressPos) && !it->canhaveTower()) {
                        QMessageBox box1(QMessageBox::NoIcon, "提示","是否升级炮塔?", QMessageBox::Yes | QMessageBox::No, NULL);
                        box1.setIconPixmap(QPixmap(":/pictures/gold.png"));
                        if(box1.exec()==QMessageBox::Yes) {
                            if ( it->getTower()->level==1 ) {
                                if (canbuyTower(2)) {
                                    Tower *tower=it->getTower()->upgrade();
                                    removeTower(it->getTower());
                                    it->sethaveTower(tower);
                                    playerGold-=Towercost[1];
                                }
                                else {
                                    QMessageBox box2(QMessageBox::NoIcon, "提示", "金币不足！");
                                    box2.setIconPixmap(QPixmap(":/pictures/remind.png"));
                                    box2.exec();
                                }
                            }
                            else if (it->getTower()->level==2) {
                                if (canbuyTower(3)) {
                                    Tower *tower=it->getTower()->upgrade();
                                    removeTower(it->getTower());
                                    it->sethaveTower(tower);
                                    playerGold-=Towercost[2];
                                }
                                else {
                                    QMessageBox box2(QMessageBox::NoIcon, "提示", "金币不足！");
                                    box2.setIconPixmap(QPixmap(":/pictures/remind.png"));
                                    box2.exec();
                                }
                            }
                            else if (it->getTower()->level==3) {
                                if (canbuyTower(4)) {
                                    Tower *tower=it->getTower()->upgrade();
                                    removeTower(it->getTower());
                                    it->sethaveTower(tower);
                                    playerGold-=Towercost[3];
                                }
                                else {
                                    QMessageBox box2(QMessageBox::NoIcon, "提示", "金币不足！");
                                    box2.setIconPixmap(QPixmap(":/pictures/remind.png"));
                                    box2.exec();
                                }
                            }
                            else if (it->getTower()->level==4) {
                                if (canbuyTower(5)) {
                                    Tower *tower=it->getTower()->upgrade();
                                    removeTower(it->getTower());
                                    it->sethaveTower(tower);
                                    playerGold-=Towercost[4];
                                }
                                else {
                                    QMessageBox box2(QMessageBox::NoIcon, "提示", "金币不足！");
                                    box2.setIconPixmap(QPixmap(":/pictures/remind.png"));
                                    box2.exec();
                                }
                            }
                            else if (it->getTower()->level==5) {
                                QMessageBox box2(QMessageBox::NoIcon, "提示", "已升至当前最高级炮塔！");
                                box2.setIconPixmap(QPixmap(":/pictures/remind.png"));
                                box2.exec();
                            }
                        }
                    }

                    ++it;
                }
            }
        }
    }
}

void MainWindow::loadTowerPositions() {
    QPoint pos[]={
        QPoint(0, 35),
        QPoint(200, 35),
        QPoint(400, 35),
        QPoint(600, 35),
        QPoint(800, 35),
        QPoint(1000, 35),

        QPoint(0, 210),
        QPoint(200, 210),
        QPoint(400, 210),
        QPoint(600, 210),
        QPoint(800, 210),
        QPoint(1000, 210),//

        QPoint(0, 385),
        QPoint(200, 385),
        QPoint(400, 385),
        QPoint(600, 385),
        QPoint(800, 385),
        QPoint(1000, 385),

        QPoint(0, 560),
        QPoint(200, 560),
        QPoint(400, 560),
        QPoint(600, 560),
        QPoint(800, 560),
        QPoint(1000, 560)
            };
    int len	= sizeof(pos) / sizeof(pos[0]);
    for (int i = 0; i < len; ++i)
        t_towerpositionlist.push_back(pos[i]);

}

void MainWindow::removeEnemy(Enemy *enemy) {
    Q_ASSERT(enemy);
    enemylist1.removeOne(enemy);
    delete enemy;

    if (enemylist1.empty()) {
        waves++;
        if (!loadWaves()) {
            if (checkpoint==2) win=true;
            else if (checkpoint==1) {
                checkpoint++;
                doGameChange();
            }
        }
    }
}

QList<Enemy *> MainWindow::enemyList1() const {
    return enemylist1;
}

QList<Enemy *> MainWindow::enemyList2() const {
    return enemylist2;
}

QList<Enemy *> MainWindow::enemyList3() const {
    return enemylist3;
}

QList<Enemy *> MainWindow::enemyList4() const {
    return enemylist4;
}

void MainWindow::removeBullet(Bullet *bullet) {
    Q_ASSERT(bullet);

    bulletlist.removeOne(bullet);
    delete bullet;
}

void MainWindow::addwaypoint() {
    waypoint *wayPoint11 = new waypoint(QPoint(10,40));
    waypointlist1.push_back(wayPoint11);

    waypoint *wayPoint12 = new waypoint(QPoint(1280, 40));
    waypointlist1.push_back(wayPoint12);
    wayPoint12->setNextWayPoint(wayPoint11);

    waypoint *wayPoint21 = new waypoint(QPoint(10, 220));
    waypointlist2.push_back(wayPoint21);

    waypoint *wayPoint22 = new waypoint(QPoint(1280, 220));
    waypointlist2.push_back(wayPoint22);
    wayPoint22->setNextWayPoint(wayPoint21);

    waypoint *wayPoint31 = new waypoint(QPoint(10,400));
    waypointlist3.push_back(wayPoint31);

    waypoint *wayPoint32 = new waypoint(QPoint(1280,400));
    waypointlist3.push_back(wayPoint32);
    wayPoint32->setNextWayPoint(wayPoint31);

    waypoint *wayPoint41 = new waypoint(QPoint(10, 580));
    waypointlist4.push_back(wayPoint41);

    waypoint *wayPoint42 = new waypoint(QPoint(1280, 580));
    waypointlist4.push_back(wayPoint42);
    wayPoint42->setNextWayPoint(wayPoint41);

}

bool MainWindow::loadWaves() {
    if (checkpoint==1) {
        if (waves>8) return  false;
        else {
            QList<waypoint *> startpoint;
            waypoint * startpoint1=waypointlist1.back();
            waypoint * startpoint2=waypointlist2.back();
            waypoint * startpoint3=waypointlist3.back();
            waypoint * startpoint4=waypointlist4.back();
            startpoint.push_back(startpoint1);
            startpoint.push_back(startpoint2);
            startpoint.push_back(startpoint3);
            startpoint.push_back(startpoint4);
            int enemyInterval[]={500, 1000, 2000, 1500, 2000, 3000, 5000, 6000};
            int p,q;
            for (int i=0;i<3;i++) {

                if (waves < 2) {
                    q=qrand() % 2;
                    Enemy *enemy=new Enemy(startpoint[0],this,1);
                    enemylist1.push_back(enemy);
                    QTimer::singleShot(enemyInterval[i],enemy,SLOT(do_Activate()));
                }
                else if (waves>=2 && waves<5) {
                    q=qrand() % 4;
                    p = qrand() % 2 + 1;
                    Enemy *enemy=new Enemy(startpoint[q],this,p);
                    enemylist1.push_back(enemy);
                    QTimer::singleShot(enemyInterval[i],enemy,SLOT(do_Activate()));
                }
                else if (waves>=5&&waves<8) {
                    q=qrand() % 4;
                    p = qrand() % 3 + 1;
                    Enemy *enemy=new Enemy(startpoint[q],this,p);
                    enemylist1.push_back(enemy);
                    QTimer::singleShot(enemyInterval[i],enemy,SLOT(do_Activate()));
                }
            }
            if (waves==8) {
                //q = qrand() % 4;
                Enemy *enemy=new Enemy(startpoint[2],this,9);
                enemylist1.push_back(enemy);
                QTimer::singleShot(enemyInterval[1],enemy,SLOT(do_Activate()));
            }
            return true;
        }
    }
    else if (checkpoint==2) {
        if (waves>12) return false;
        else {
            QList<waypoint *> startpoint;
            waypoint * startpoint1=waypointlist1.back();
            waypoint * startpoint2=waypointlist2.back();
            waypoint * startpoint3=waypointlist3.back();
            waypoint * startpoint4=waypointlist4.back();
            startpoint.push_back(startpoint1);
            startpoint.push_back(startpoint2);
            startpoint.push_back(startpoint3);
            startpoint.push_back(startpoint4);
            int enemyInterval[]={500, 1000, 2000, 1500, 2000, 3000, 5000, 6000};
            int p,q;
            for (int i=0;i<6;i++) {
                if (waves<=5) {
                    q = qrand() % 3 ;
                    p = qrand() % 3 + 1;
                    Enemy *enemy=new Enemy(startpoint[q],this,p);
                    enemylist1.push_back(enemy);
                    QTimer::singleShot(enemyInterval[i],enemy,SLOT(do_Activate()));
                }
                else if (waves>5&&waves<10) {
                    q = qrand() % 4;
                    p= qrand() % 4 + 1;
                    Enemy *enemy=new Enemy(startpoint[q],this,p);
                    enemylist1.push_back(enemy);
                    QTimer::singleShot(enemyInterval[i],enemy,SLOT(do_Activate()));
                }
                else if (waves>=10&&waves<12) {
                    q = qrand() % 4;
                    p= qrand() % 5 + 1;
                    Enemy *enemy=new Enemy(startpoint[q],this,p);
                    enemylist1.push_back(enemy);
                    QTimer::singleShot(enemyInterval[i],enemy,SLOT(do_Activate()));
                }
            }
            if (waves==12) {
                Enemy *enemy=new Enemy(startpoint[3],this,10);
                enemylist1.push_back(enemy);
                QTimer::singleShot(enemyInterval[1],enemy,SLOT(do_Activate()));
            }
            return true;
        }
    }
}

void MainWindow::getHpDamage(int damage/* = 1*/)
{
    //audioPlayer->playSound(LifeLoseSound);
    playerHp -= damage;
    if (playerHp <= 0)
    {
        doGameOver();

    }
}

void MainWindow::doGameOver()
{
//    QPainter painter;
//    painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/pictures/youlose.jpg"));
//    if (!lose)
//    {

        // 此处应该切换场景到结束场景并结束游戏
        lose=true;
        QMediaPlayer * player = new QMediaPlayer;
        player->setMedia(QUrl("qrc:/sound/gameover.mp3"));
        player->setVolume(30);
        player->play();
//        QApplication* app;
//        app->exit(0);

//    }
}

void MainWindow::updateMap() {
    foreach (Enemy *enemy1, enemylist1)
        enemy1->move();
    foreach (Enemy *enemy2, enemylist2)
        enemy2->move();
    foreach (Enemy *enemy3, enemylist3)
        enemy3->move();
    foreach (Enemy *enemy4, enemylist4)
        enemy4->move();
    foreach (Tower *tower, t_towerlist)
        tower->checkEnemyInRange();

    this->removeDeath();
    this->act();
    if (qrand() % 200 < 1)
    {
        Bonus* sun = new zSunFall(this);
        sun->setgame(this);
        Bonuses.append(sun);
    }
    this->exit->raise();

    update();
}

void MainWindow::addBullet(Bullet *bullet) {
    Q_ASSERT(bullet);

    bulletlist.push_back(bullet);
}


bool MainWindow::canbuyTower(int level) const {
    if (Towercost[level-1]<=playerGold) return true;
    else return false;
}

void MainWindow::awardGold(int gold) {
    playerGold+=gold;
    update();
}

void MainWindow::drawWave(QPainter *painter)
{
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("黑体", 20, QFont::Bold, false));
    painter->drawPixmap(450,750,350,150,QPixmap(":/pictures/wave_1.png"));
    painter->drawText(QRect(600,770,200,100), QString("%1").arg(waves));
}

void MainWindow::drawHP(QPainter *painter)
{
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("黑体", 20, QFont::Bold, false));
    painter->drawPixmap(0,750,350,150,QPixmap(":/pictures/blood_1.png"));
    painter->drawText(QRect(150,770,200,100), QString("%1").arg(playerHp));
}

void MainWindow::drawPlayerGold(QPainter *painter)
{
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("黑体", 20, QFont::Bold, false));
    painter->drawPixmap(900,750,350,150,QPixmap(":/pictures/gold_1.png"));
    painter->drawText(QRect(1050,770,200,100), QString("%1").arg(playerGold));
}

void MainWindow::drawCheckPoint(QPainter *painter)
{
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("黑体", 20, QFont::Bold, false));
    painter->drawPixmap(1110,750,200,150,QPixmap(":/pictures/ui2.png"));
    painter->drawText(QRect(520,770,200,100), QString("第一关"));
}

void MainWindow::gameStart()
{
    if(!lose)
    loadWaves();
}

void MainWindow::removeDeath() {
    int p = 0;
    while (p < Bonuses.count())
    {
        if (!(Bonuses[p]->alive))
        {
            delete (Bonuses[p]);
            Bonuses.removeAt(p);
        }
        else
        {
            p++;
        }
    }
}

void MainWindow::act() {
    Bonus* i;
    foreach(i, Bonuses)
    {
        i->act();
    }
}

void MainWindow::removeTower(Tower *tower) {
    Q_ASSERT(tower);
    t_towerlist.removeOne(tower);
    delete tower;
}

void MainWindow::addTower(Tower *tower) {
    Q_ASSERT(tower);
    t_towerlist.push_back(tower);
    update();
}

void MainWindow::doGameChange() {
    waves=1;
    playerHp=20;
    //打印关卡转换界面
    QPainter painter;
    painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/pictures/ghost.jpg"));

    QMessageBox box1(QMessageBox::NoIcon, "提示","恭喜通过第一关！是否进入第二关？", QMessageBox::Yes | QMessageBox::No, NULL);
    box1.setIconPixmap(QPixmap(":/pictures/next.png"));
    if(box1.exec()==QMessageBox::Yes) gameStart();
    else {
        QApplication* app;
        app->exit(0);
    }
}
