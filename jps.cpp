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

    // CallSearch(curPosx, curPosY);
    // �ݹ������� ||  �������յ�
    if (depth == 0 || (end.X == curPosx && end.Y == curPosY))
        return new Point(curPosx, curPosY);

    // �Խ���
    if (xDirection != 0 && yDirection != 0)
    {
        if ((IsWalkable(curPosx + xDirection, curPosY - yDirection) && !IsWalkable(curPosx, curPosY - yDirection)) || (IsWalkable(curPosx - xDirection, curPosY + yDirection) &&
                                                                                                                       !IsWalkable(curPosx - xDirection, curPosY)))
        {
            return new Point(curPosx, curPosY);
        }
        // ����ݹ�Ѱ��ǿ���ھ�
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
    else if (xDirection != 0)
    {
        // ����
        if ((IsWalkable(curPosx + xDirection, curPosY + 1) && !IsWalkable(curPosx, curPosY + 1)) || (IsWalkable(curPosx + xDirection, curPosY - 1) && !IsWalkable(curPosx, curPosY - 1)))
        {
            return new Point(curPosx, curPosY);
        }
    }
    else if (yDirection != 0)
    {
        // ����
        if ((IsWalkable(curPosx + 1, curPosY + yDirection) && !IsWalkable(curPosx + 1, curPosY)) || (IsWalkable(curPosx - 1, curPosY + yDirection) && !IsWalkable(curPosx - 1, curPosY)))
        {
            return new Point(curPosx, curPosY);
        }
    }
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
        // �����parent��ΪNULL�������ھӷ��ϰ�����롣

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
        // 判断前一步是斜着走还是直线走的，如果是直线走的话，yDirection  xDirection不同是为0
        int xDirection = Clamp(point->X - parent->X, -1, 1);
        int yDirection = Clamp(point->Y - parent->Y, -1, 1);
        if (xDirection != 0 && yDirection != 0)
        {
            // 如果是斜着走，下一步还是斜着或者向着前一布运行方向的大体方向
            bool neighbourForward = IsWalkable(point->X, point->Y + yDirection); // 下
            bool neighbourRight = IsWalkable(point->X + xDirection, point->Y);   // 右
            bool neighbourLeft = IsWalkable(point->X - xDirection, point->Y);    // 左
            bool neighbourBack = IsWalkable(point->X, point->Y - yDirection);    // 上
            if (neighbourForward)                                                // 下
            {
                points.push_back(new Point(point->X, point->Y + yDirection));
            }
            if (neighbourRight) // 右
            {
                points.push_back(new Point(point->X + xDirection, point->Y));
            }
            if ((neighbourForward || neighbourRight) && IsWalkable(point->X + xDirection, point->Y + yDirection))
            {
                points.push_back(new Point(point->X + xDirection, point->Y + yDirection));
            }
            // ǿ���ھӵĴ���
            if (!neighbourLeft && neighbourForward)
            {
                if (IsWalkable(point->X - xDirection, point->Y + yDirection))
                {
                    points.push_back(new Point(point->X - xDirection, point->Y + yDirection));
                }
            }
            if (!neighbourBack && neighbourRight)
            {
                if (IsWalkable(point->X + xDirection, point->Y - yDirection))
                {
                    points.push_back(new Point(point->X + xDirection, point->Y - yDirection));
                }
            }
        }
        else
        {
            if (xDirection == 0)
            {
                // ����
                if (IsWalkable(point->X, point->Y + yDirection))
                {
                    points.push_back(new Point(point->X, point->Y + yDirection));
                    // ǿ���ھ�
                    if (!IsWalkable(point->X + 1, point->Y) && IsWalkable(point->X + 1, point->Y + yDirection))
                    {
                        points.push_back(new Point(point->X + 1, point->Y + yDirection));
                    }
                    if (!IsWalkable(point->X - 1, point->Y) && IsWalkable(point->X - 1, point->Y + yDirection))
                    {
                        points.push_back(new Point(point->X - 1, point->Y + yDirection));
                    }
                }
            }
            else
            {
                // ����
                if (IsWalkable(point->X + xDirection, point->Y))
                {
                    points.push_back(new Point(point->X + xDirection, point->Y));
                    // ǿ���ھ�
                    if (!IsWalkable(point->X, point->Y + 1) && IsWalkable(point->X + xDirection, point->Y + 1))
                    {
                        points.push_back(new Point(point->X + xDirection, point->Y + 1));
                    }
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
        cout << "cx : " << curPoint->X << "     y : " << curPoint->Y << endl;
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
                xDirection = 1;
                yDirection = 1;
            }

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
    Point end(6, 10);

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
