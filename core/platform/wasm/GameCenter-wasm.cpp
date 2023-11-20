/****************************************************************************
Copyright (c) 2021-2023 Bytedance Inc.

https://axmolengine.github.io/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is/Users/max/Desktop/the-fit/axmol/core/platform/linux/GameCenter-linux.cpp
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "platform/GameCenter.h"

NS_AX_BEGIN

void GameCenter::authenticatePlayer() {}

void GameCenter::setListener(GameCenterListener* listener) {}

void GameCenter::removeListener() {}

void GameCenter::showLeaderboard(const std::string& leaderboardIdentifier) {}

void GameCenter::showAllLeaderboards() {}

void GameCenter::showAchievements() {}

void GameCenter::getMyScore(const std::string& leaderboardIdentifier) {}

void GameCenter::submitScore(const std::string& leaderboardIdentifier, long score) {}

void GameCenter::unlockAchievement(const std::string& achievementIdentifier) {}

void GameCenter::setAchievementSteps(const std::string& achievementIdentifier, double steps) {}

NS_AX_END
