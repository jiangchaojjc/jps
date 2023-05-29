// jps.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <stdio.h>
#include <string>
#include <list>
#include <vector>
#include <cmath>
using namespace std;

std::vector<std::vector<int>> map = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                                     {1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1},
                                     {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
                                     {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1},
                                     {1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1},
                                     {1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
                                     {1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
                                     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

template <typename T>
constexpr T Clamp(const T &val, const T &low, const T &high)
{
    return val < low ? low : (high < val ? high : val);
}

struct Point
{
    int X, Y;                                                                 // �����꣬����Ϊ�˷��㰴��C++�����������㣬x�������ţ�y��������
    int F, G, H;                                                              // F=G+H
    Point *ParentPoint;                                                       // parent�����꣬����û����ָ�룬�Ӷ��򻯴���
    Point(int _x, int _y) : X(_x), Y(_y), F(0), G(0), H(0), ParentPoint(NULL) // ������ʼ��
    {
    }
};

std::list<Point *> openList;
std::list<Point *> closeList;

int CalcG(Point *start, Point *point)
{
    int distX = abs(point->X - start->X);
    int distY = abs(point->Y - start->Y);
    int G = 0;
    if (distX > distY)
        G = 14 * distY + 10 * (distX - distY);
    else
        G = 14 * distX + 10 * (distY - distX);

    int parentG = point->ParentPoint != NULL ? point->ParentPoint->G : 0;
    return G + parentG;
}

int CalcH(Point *end, Point *point)
{
    int step = abs(point->X - end->X) + abs(point->Y - end->Y);
    return step * 10;
}

int CalcF(Point *point)
{
    return point->G + point->H;
}

bool IsWalkable(int x, int y)
{
    return x >= 0 && x < map.size() && y >= 0 && y < map[1].size() && (map[x][y] != 1);
}

Point *Jump(int curPosx, int curPosY, int xDirection, int yDirection, int depth, Point end)
{
    if (!IsWalkable(curPosx, curPosY))
    {
        // cout << "can not run " << endl;
        return NULL;
    }

    // CallSearch(curPosx, curPosY); //搜索到这的步长，包括没有走的邻居点，看效率的
    // 到达目标点或者到达遍历深度
    if (depth == 0 || (end.X == curPosx && end.Y == curPosY))
        return new Point(curPosx, curPosY);

    // �Խ���
    if (xDirection != 0 && yDirection != 0)
    {
        // 右上行，上不行 ; 左下行，左不行。斜着走的情况，与斜着的方向夹角为90度的方向能走，但是与斜着的方向135度的方向不能走，那么当前点就是跳点，找到跳点就返回
        if ((IsWalkable(curPosx + xDirection, curPosY - yDirection) && !IsWalkable(curPosx, curPosY - yDirection)) || (IsWalkable(curPosx - xDirection, curPosY + yDirection) &&
                                                                                                                       !IsWalkable(curPosx - xDirection, curPosY)))
        {
            return new Point(curPosx, curPosY);
        }
        //
        if (Jump(curPosx + xDirection, curPosY, xDirection, 0, depth - 1, end) != NULL)
        {
            return new Point(curPosx, curPosY);
        }

        // ������ݹ�Ѱ��ǿ���ھ�
        if (Jump(curPosx, curPosY + yDirection, 0, yDirection, depth - 1, end) != NULL)
        {
            return new Point(curPosx, curPosY);
        }
    }
    // 取跳点时时，如有左右两边不能，但是与当前方向斜着方向能走，那么斜着的方向就是跳点
    else if (xDirection != 0) // 垂直方向不能走，水平方向走
    {
        // 右下能走，下不能走， 右上能走，上不能走
        if ((IsWalkable(curPosx + xDirection, curPosY + 1) && !IsWalkable(curPosx, curPosY + 1)) || (IsWalkable(curPosx + xDirection, curPosY - 1) && !IsWalkable(curPosx, curPosY - 1)))
        {
            return new Point(curPosx, curPosY);
        }
    }
    else if (yDirection != 0) // 垂直方向走
    {
        // 右下能走，右不能走，左下能走，左不能走
        if ((IsWalkable(curPosx + 1, curPosY + yDirection) && !IsWalkable(curPosx + 1, curPosY)) || (IsWalkable(curPosx - 1, curPosY + yDirection) && !IsWalkable(curPosx - 1, curPosY)))
        {
            return new Point(curPosx, curPosY);
        }
    }
    // 如果斜着行走，斜着行走也会走水平方向和垂直方向的跳点，寻找方式按上面的一样，找到之后返回找到的条点给初始调用的地方。
    return Jump(curPosx + xDirection, curPosY + yDirection, xDirection, yDirection, depth - 1, end);
}

std::list<Point *> GetNeighbors(Point *point)
{
    std::list<Point *> points;
    static int first = 0;
    if (first == 0)
    {
        first = 1;
        // ��ȡ�˵���ھ�
        // 第一步的时候，四个方向都去找

        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                if (x == 0 && y == 0)
                    continue;

                if (IsWalkable(x + point->X, y + point->Y))
                {
                    points.push_back(new Point(x + point->X, y + point->Y));
                }
            }
        }
        return points;
    }
    else
    {
        Point *parent = point->ParentPoint;
        // 判断前一步是斜着走还是直线走的，如果是直线走的话，yDirection  xDirection不同时为0
        int xDirection = Clamp(point->X - parent->X, -1, 1);
        int yDirection = Clamp(point->Y - parent->Y, -1, 1);
        if (xDirection != 0 && yDirection != 0)
        {
            // 如果是斜着走，下一步还是斜着或者向着前一布运行方向的大体方向
            bool neighbourForward = IsWalkable(point->X, point->Y + yDirection); // 下
            bool neighbourRight = IsWalkable(point->X + xDirection, point->Y);   // 右
            bool neighbourLeft = IsWalkable(point->X - xDirection, point->Y);    // 左
            bool neighbourBack = IsWalkable(point->X, point->Y - yDirection);    // 上

            // 1.如果前进的方向，可以斜向，如果斜向的方向周围45度的方向能走，那么周围45方向也是邻居点。
            // 2.从斜向方向的周围45度再往外扩90，如果该方向不能走，但是从斜向方向周围45度再往外扩45度能走，那么该角度方向也是邻居点。
            // 3.不可能出现只有某个方向为负xDirection或者只有负yDirection
            if (neighbourForward) // 下
            {
                points.push_back(new Point(point->X, point->Y + yDirection)); // 取下
            }
            if (neighbourRight) // 取右
            {
                points.push_back(new Point(point->X + xDirection, point->Y));
            }
            if ((neighbourForward || neighbourRight) && IsWalkable(point->X + xDirection, point->Y + yDirection)) // 右行或下行并且右下行，取右下
            {
                points.push_back(new Point(point->X + xDirection, point->Y + yDirection));
            }
            // 左边不行，下边行，左下也行，取左下
            if (!neighbourLeft && neighbourForward)
            {
                if (IsWalkable(point->X - xDirection, point->Y + yDirection))
                {
                    points.push_back(new Point(point->X - xDirection, point->Y + yDirection));
                }
            }
            // 上不行，右边行，右上行，取值右上
            if (!neighbourBack && neighbourRight)
            {
                if (IsWalkable(point->X + xDirection, point->Y - yDirection))
                {
                    points.push_back(new Point(point->X + xDirection, point->Y - yDirection));
                }
            }
        }
        else // 取邻居点时，如果是直行，当强方向点必须能走，如有左右两边不能，但是与当前方向斜着方向能走，那么斜着的方向也是邻居点
        {
            if (xDirection == 0) // 垂直方向行走
            {
                // 下行，取下。如果是直线行走，那么这个方向爱那个必须能再次能走，不然，这个方向就是死路方向
                if (IsWalkable(point->X, point->Y + yDirection))
                {
                    points.push_back(new Point(point->X, point->Y + yDirection));

                    // 下行，右边不行，右下角行，取右下角
                    if (!IsWalkable(point->X + 1, point->Y) && IsWalkable(point->X + 1, point->Y + yDirection))
                    {
                        points.push_back(new Point(point->X + 1, point->Y + yDirection));
                    }
                    // 下行，左不行，左下角行，取左下角
                    if (!IsWalkable(point->X - 1, point->Y) && IsWalkable(point->X - 1, point->Y + yDirection))
                    {
                        points.push_back(new Point(point->X - 1, point->Y + yDirection));
                    }
                }
            }
            else // 水平方向行走
            {
                // 右边行，取右边。如果是直线行走，那么这个方向必须能再次能走，不然，这个方向就是死路方向
                if (IsWalkable(point->X + xDirection, point->Y))
                {
                    points.push_back(new Point(point->X + xDirection, point->Y));
                    // 右边行，下不行，右下行，取右下
                    if (!IsWalkable(point->X, point->Y + 1) && IsWalkable(point->X + xDirection, point->Y + 1))
                    {
                        points.push_back(new Point(point->X + xDirection, point->Y + 1));
                    }
                    // 右边行，上边不行，右上行，取右上
                    if (!IsWalkable(point->X, point->Y - 1) && IsWalkable(point->X + xDirection, point->Y - 1))
                    {
                        points.push_back(new Point(point->X + xDirection, point->Y - 1));
                    }
                }
            }
        }
    }

    return points;
}

Point *getLeastFpoint()
{
    if (!openList.empty())
    {
        auto resPoint = openList.front();
        for (auto &point : openList)
        {
            if (point->F < resPoint->F)
            {
                resPoint = point;
            }
        }
        return resPoint;
    }
    return NULL;
}

Point *isInList(std::list<Point *> lists, Point *point)
{
    for (auto p : lists)
    {
        if (p->X == point->X && p->Y == point->Y)
        {
            return p;
        }
    }
    return NULL;
}

Point *findPath(Point &startPoint, Point &endPoint)
{
    openList.push_back(new Point(startPoint.X, startPoint.Y)); // �������,��������һ���ڵ㣬�������
    while (!openList.empty())
    {
        auto curPoint = getLeastFpoint(); // �ҵ�Fֵ��С�ĵ�
        cout << "cx : " << curPoint->X << "     cy : " << curPoint->Y << endl;
        openList.remove(curPoint);     // �ӿ����б���ɾ��
        closeList.push_back(curPoint); // �ŵ��ر��б�
        // 1,�ҵ���ǰ��Χ�˸����п���ͨ���ĸ���
        auto surroundPoints = GetNeighbors(curPoint);
        int xDirection, yDirection;
        for (auto &target : surroundPoints)
        {
            if (curPoint->ParentPoint != NULL)
            {
                xDirection = Clamp(target->X - curPoint->X, -1, 1);
                yDirection = Clamp(target->Y - curPoint->Y, -1, 1);
            }
            else
            {
                xDirection = 0;
                yDirection = 0;
            }
            cout << "nx : " << target->X << "     ny : " << target->Y << endl;
            cout << "xDirection : " << xDirection << "     yDirection : " << yDirection << endl;
            // 搜索到跳点之后，记录所有的跳点在openlist,取openlist中带价值最小的点作为当前点
            Point *jumpPoint = Jump(target->X, target->Y, xDirection, yDirection, 10, endPoint);

            if (jumpPoint != NULL)
            {
                cout << "jx : " << jumpPoint->X << "     jy : " << jumpPoint->Y << endl;
                if (!isInList(closeList, jumpPoint))
                {
                    jumpPoint->ParentPoint = curPoint;

                    jumpPoint->G = CalcG(curPoint, jumpPoint);
                    jumpPoint->H = CalcH(jumpPoint, &endPoint);
                    jumpPoint->F = CalcF(jumpPoint);
                }
                // 3����ĳһ�����ӣ����ڿ����б��У�����Gֵ, �����ԭ���Ĵ�, ��ʲô������, �����������ĸ��ڵ�Ϊ��ǰ��,������G��F
                auto oldjump = isInList(openList, jumpPoint);
                if (oldjump != NULL)
                {
                    if (jumpPoint->G < oldjump->G)
                    {
                        oldjump->ParentPoint = curPoint;

                        oldjump->G = jumpPoint->G;
                        oldjump->F = CalcF(jumpPoint);
                    }
                }
                else
                {
                    openList.push_back(jumpPoint);
                }
                Point *resPoint = isInList(openList, &endPoint);
                if (resPoint)
                    return resPoint; // �����б���Ľڵ�ָ�룬��Ҫ��ԭ�������endpointָ�룬��Ϊ���������
            }
        }
    }

    return NULL;
}

std::list<Point *> GetPath(Point &startPoint, Point &endPoint)
{
    Point *result = findPath(startPoint, endPoint);
    std::list<Point *> path;
    while (result)
    {
        path.push_front(result);
        result = result->ParentPoint;
    }
    openList.clear();
    closeList.clear();
    return path;
}

bool Inpath(int x, int y, std::list<Point *> path)
{
    for (auto p : path)
    {
        if (p->X == x && p->Y == y)
        {
            return true;
        }
    }
}

int main(int argc, char *argv[])
{

    for (int i = 0; i < map.size(); i++)
    {
        for (int j = 0; j < map[i].size(); j++)
        {
            std::cout << map[i][j] << " ";
        }
        std::cout << std::endl;
    };

    Point start(1, 1);
    Point end(1, 10);

    std::list<Point *> path = GetPath(start, end);

    for (int i = 0; i < map.size(); i++)
    {
        for (int j = 0; j < map[i].size(); j++)
        {

            if (Inpath(i, j, path))
            {
                if (map[i][j] != 0)
                {
                    std::cout << "e"
                              << " ";
                }
                else
                {
                    std::cout << "*"
                              << " ";
                }
            }
            else
            {
                std::cout << map[i][j] << " ";
            }
        }
        std::cout << std::endl;
    };

    return 0;
}
