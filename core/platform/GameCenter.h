/****************************************************************************
Copyright (c) 2021 Bytedance Inc.

https://axmolengine.github.io/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
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

#ifndef __AXGAMECENTER_H__
#define __AXGAMECENTER_H__

NS_AX_BEGIN

/**
 * @addtogroup platform
 * @{
 */

enum GameCenterConnectionStatus {
    Connected = 0,
    Disconnected,
    ConnectionError
};

/**
 * @class GameCenterListener
 * @brief
 */
class GameCenterListener {
public:
    /**
     * Call method invoked when the connection changes its status.
     * Values are as follows:
     *   + Connected:         successfully connected.
     *   + Disconnected:     successfully disconnected.
     *   + ConnectionError: error with Game Center or Google Play services connection.
     */
    virtual void onConnectionStatusChanged(GameCenterConnectionStatus status) = 0;

    /**
     * Callback methods invoked when an score has been successfully submitted to a leaderboard.
     * It notifies back with the `leaderboardIdentifier` and the submitted score.
     */
    virtual void onScoreSubmitted(const std::string& leaderboardIdentifier, long score) = 0;

    virtual void onScoreSubmitError(const std::string& leaderboardIdentifier, long score, int errorCode, const std::string& errorDescription) {};

    /**
     * Callback methods invoked from a call to `getMyScore` method.
     */
    virtual void onMyScore(const std::string& leaderboardIdentifier, long score) {};

    virtual void onMyScoreError(const std::string& leaderboardIdentifier, int errorCode, const std::string& errorDescription) {};

    /**
     * Callback method invoked when the request call to `setAchievementSteps` method is successful and
     * that achievement gets unlocked. This happens when the incremental step count reaches its maximum value.
     * Maximum step count for an incremental achievement is defined in the google play developer console.
     */
    virtual void onIncrementalAchievementUnlocked(const std::string& achievementIdentifier) = 0;

    /**
     * Callback methods invoked from a call to `setAchievementSteps` method.
     */
    virtual void onIncrementalAchievementStep(const std::string& achievementIdentifier, double step) = 0;

    virtual void onIncrementalAchievementStepError(const std::string& achievementIdentifier, double steps, int errorCode, const std::string& errorDescription) {};

    /**
     * Callback methods invoked from a call to `unlockAchievement` method.
     */
    virtual void onAchievementUnlocked(const std::string& achievementIdentifier) = 0;

    virtual void onAchievementUnlockError(const std::string& achievementIdentifier, int errorCode, const std::string& errorDescription) {};
};

/**
 * @class GameCenter
 * @brief
 */
class AX_DLL GameCenter
{
public:

    /**
     * Authenticate player
     */
    static void authenticatePlayer();

    /**
     * Set listener for GameCenter
     */
    static void setListener(GameCenterListener* listener);

    /**
     * Remove listener for GameCenter
     */
    static void removeListener();

    /**
     * Show leaderboard with identifier
     */
    static void showLeaderboard(const std::string& leaderboardIdentifier);

    /**
     * Show all leaderboards
     */
    static void showAllLeaderboards();

    /**
     * Show achievements
     */
    static void showAchievements();

    /**
     * Get player score from leaderboard
     */
    static void getMyScore(const std::string& leaderboardIdentifier);

    /**
     * Submit player score to leaderboard
     */
    static void submitScore(const std::string& leaderboardIdentifier, long score);

    /**
     * Unlock achievement with identifier
     */
    static void unlockAchievement(const std::string& achievementIdentifier);

    /**
     * Set steps of achievement with identifier. On iOS this is a percentage value
     */
    static void setAchievementSteps(const std::string& achievementIdentifier, double steps);

private:
    AX_DISALLOW_IMPLICIT_CONSTRUCTORS(GameCenter);
};

// end group
/// @}

NS_AX_END

#endif /* __AXGAMECENTER_H__ */
