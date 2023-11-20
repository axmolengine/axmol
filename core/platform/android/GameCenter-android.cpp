/****************************************************************************
Copyright (c) 2021-2023 Bytedance Inc.

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
#include "platform/GameCenter.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"

NS_AX_BEGIN

static const char* helperClassName = "org.axmol.lib.AxmolEngine";

static GameCenterListener* gcListener;

std::string jstringToString(JNIEnv *env, jstring str) {
    return env->GetStringUTFChars(str, NULL);
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onConnectionStatusChanged(JNIEnv* env, jobject thiz, jint status)
{
    if(gcListener == nullptr) {
        return;
    }
    gcListener->onConnectionStatusChanged(static_cast<GameCenterConnectionStatus>(status));
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onScoreSubmitted(JNIEnv* env, jobject thiz, jstring leaderboardIdentifier, jlong score)
{
    gcListener->onScoreSubmitted(jstringToString(env, leaderboardIdentifier), score);
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onScoreSubmitError(JNIEnv* env, jobject thiz, jstring leaderboardIdentifier, jlong score, jint errorCode, jstring errorDescription)
{
    gcListener->onScoreSubmitError(jstringToString(env, leaderboardIdentifier), score, errorCode, jstringToString(env, errorDescription));
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onMyScore(JNIEnv* env, jobject thiz, jstring leaderboardIdentifier, jlong score)
{
    gcListener->onMyScore(jstringToString(env, leaderboardIdentifier), score);
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onMyScoreError(JNIEnv* env, jobject thiz, jstring leaderboardIdentifier, jint errorCode, jstring errorDescription)
{
    gcListener->onMyScoreError(jstringToString(env, leaderboardIdentifier), errorCode, jstringToString(env, errorDescription));
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onIncrementalAchievementUnlocked(JNIEnv* env, jobject thiz, jstring achievementIdentifier)
{
    gcListener->onIncrementalAchievementUnlocked(jstringToString(env, achievementIdentifier));
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onIncrementalAchievementStep(JNIEnv* env, jobject thiz, jstring achievementIdentifier, jdouble steps)
{
    gcListener->onIncrementalAchievementStep(jstringToString(env, achievementIdentifier), steps);
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onIncrementalAchievementStepError(JNIEnv* env, jobject thiz, jstring achievementIdentifier, jdouble steps, jint errorCode, jstring errorDescription)
{
    gcListener->onIncrementalAchievementStepError(jstringToString(env, achievementIdentifier), steps, errorCode, jstringToString(env, errorDescription));
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onAchievementUnlocked(JNIEnv* env, jobject thiz, jstring achievementIdentifier)
{
    gcListener->onAchievementUnlocked(jstringToString(env, achievementIdentifier));
}

extern "C" JNIEXPORT void JNICALL Java_org_axmol_lib_GameServicesManager_onAchievementUnlockError(JNIEnv* env, jobject thiz, jstring achievementIdentifier, jint errorCode, jstring errorDescription)
{
    gcListener->onAchievementUnlockError(jstringToString(env, achievementIdentifier), errorCode, jstringToString(env, errorDescription));
}

void GameCenter::authenticatePlayer()
{
    JniHelper::callStaticVoidMethod(helperClassName, "authenticatePlayer");
}

void GameCenter::setListener(GameCenterListener* listener)
{
    gcListener = listener;
}

void GameCenter::removeListener()
{
    gcListener = nullptr;
}

void GameCenter::showLeaderboard(const std::string& leaderboardIdentifier)
{
    JniHelper::callStaticVoidMethod(helperClassName, "showLeaderboard", leaderboardIdentifier);
}

void GameCenter::showAllLeaderboards()
{
    JniHelper::callStaticVoidMethod(helperClassName, "showAllLeaderboards");
}

void GameCenter::showAchievements()
{
    JniHelper::callStaticVoidMethod(helperClassName, "showAchievements");
}

void GameCenter::getMyScore(const std::string& leaderboardIdentifier)
{
    JniHelper::callStaticVoidMethod(helperClassName, "getMyScore", leaderboardIdentifier);
}

void GameCenter::submitScore(const std::string& leaderboardIdentifier, long score)
{
    JniHelper::callStaticVoidMethod(helperClassName, "submitScore", leaderboardIdentifier, score);
}

void GameCenter::unlockAchievement(const std::string& achievementIdentifier)
{
    JniHelper::callStaticVoidMethod(helperClassName, "unlockAchievement", achievementIdentifier);
}

void GameCenter::setAchievementSteps(const std::string& achievementIdentifier, double steps)
{
    JniHelper::callStaticVoidMethod(helperClassName, "setAchievementSteps", achievementIdentifier, steps);
}

NS_AX_END
