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
#import <GameKit/GameKit.h>

#include "platform/GameCenter.h"

@interface GameCenterManager : NSObject <GKGameCenterControllerDelegate> {
    ax::GameCenterListener* _listener;
}

+ (id)sharedGameCenterManager;
- (void)authenticatePlayer;
- (void)showLeaderboard:(NSString *)leaderboardIdentifier;
- (void)showAllLeaderboards;
- (void)showAchievements;

@end

@implementation GameCenterManager

#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
typedef NSViewController ViewController;

static ViewController* _contentViewController;

- (ViewController*)rootViewController
{
    if (_contentViewController == NULL)
    {
        _contentViewController = [[ViewController alloc] init];
        auto contentView = [NSApplication sharedApplication].keyWindow.contentView;
        [contentView addSubview:_contentViewController.view];
    }
    return _contentViewController;
}

- (void)presentViewController:(ViewController*)viewController
{
    [[self rootViewController] presentViewControllerAsSheet:viewController];
}

- (void)dismissViewController:(ViewController*)viewController
{
    [viewController dismissViewController: viewController];
}
#else
typedef UIViewController ViewController;

- (UIViewController*)rootViewController
{
    return [UIApplication sharedApplication].keyWindow.rootViewController;
}

- (void)presentViewController:(ViewController*)viewController
{
    [[self rootViewController] presentViewController:viewController animated:YES completion:{}];
}

- (void)dismissViewController:(ViewController*)viewController
{
    [viewController dismissViewControllerAnimated:YES completion:{}];
}
#endif

static GameCenterManager* s_pGameCenterManager;

+ (id)sharedGameCenterManager
{
    if (s_pGameCenterManager == nil)
    {
        s_pGameCenterManager = [[self alloc] init];
    }

    return s_pGameCenterManager;
}

- (id)init
{
    return self;
}

- (void)dealloc
{
    s_pGameCenterManager = nullptr;
    _listener = nullptr;
    [super dealloc];
}

- (void)authenticatePlayer
{
    auto localPlayer = [GKLocalPlayer localPlayer];
    [localPlayer setAuthenticateHandler: ^(ViewController *viewController, NSError *error) {
        if (viewController != nil) {
            [self presentViewController:viewController];
        }
        else if (_listener) {
            if (localPlayer.authenticated) {
                _listener->onConnectionStatusChanged(ax::GameCenterConnectionStatus::Connected);
            }
            else if (error) {
                NSLog(@"Game Center authentication error: %@", error);
                _listener->onConnectionStatusChanged(ax::GameCenterConnectionStatus::ConnectionError);
            }
        }
    }];
}

-(void)setListener:(ax::GameCenterListener*)listener
{
    _listener = listener;
}

-(void)removeListener
{
    _listener = nullptr;
}

- (void)showLeaderboard:(NSString *)leaderboardIdentifier
{
    auto gcViewController = [[GKGameCenterViewController alloc] init];
    gcViewController.gameCenterDelegate = self;
    gcViewController.viewState = GKGameCenterViewControllerStateLeaderboards;
    gcViewController.leaderboardIdentifier = leaderboardIdentifier;

    [self presentViewController:gcViewController];
}

- (void)showAllLeaderboards
{
    auto gcViewController = [[GKGameCenterViewController alloc] init];
    gcViewController.gameCenterDelegate = self;
    gcViewController.viewState = GKGameCenterViewControllerStateLeaderboards;

    [self presentViewController:gcViewController];
}

- (void)showAchievements
{
    auto gcViewController = [[GKGameCenterViewController alloc] init];
    gcViewController.gameCenterDelegate = self;
    gcViewController.viewState = GKGameCenterViewControllerStateAchievements;

    [self presentViewController:gcViewController];
}

- (void)getMyScoreFromLeaderboardWithIdentifier:(NSString *)leaderboardIdentifier
{
    auto leaderboardRequest = [[GKLeaderboard alloc] initWithPlayers:@[GKLocalPlayer.localPlayer]];
    leaderboardRequest.identifier = leaderboardIdentifier;
    leaderboardRequest.timeScope = GKLeaderboardTimeScopeAllTime;
    leaderboardRequest.playerScope = GKLeaderboardPlayerScopeGlobal;
    [leaderboardRequest loadScoresWithCompletionHandler:^(NSArray *scores, NSError *error) {
        if (!_listener) {
            return;
        }

        if (error) {
            _listener->onMyScoreError(leaderboardIdentifier.UTF8String, (int)error.code, error.localizedDescription.UTF8String);
        }
        else {
            auto localPlayerScore = leaderboardRequest.localPlayerScore;
            _listener->onMyScore(leaderboardIdentifier.UTF8String, localPlayerScore.value);
        }
    }];
}

- (void)submitScore:(long)score forLeaderboardIdentifier:(NSString *)leaderboardIdentifier
{
    auto gScore = [[GKScore alloc] initWithLeaderboardIdentifier:leaderboardIdentifier];
    gScore.value = score;
    gScore.context = 0;

    [GKScore reportScores:@[gScore] withCompletionHandler:^(NSError *error) {
        if (!_listener) {
            return;
        }

        if (error) {
            _listener->onScoreSubmitError(leaderboardIdentifier.UTF8String, score, (int)error.code, error.localizedDescription.UTF8String);
        }
        else {
            _listener->onScoreSubmitted(leaderboardIdentifier.UTF8String, score);
        }
    }];
}

- (void)reportAchievement:(NSString *)achievementIdentifier withPercentComplete:(double)percentComplete andCompletionHandler:(void (^)(NSError*))completionHandler
{
    auto achievement = [[GKAchievement alloc] initWithIdentifier:achievementIdentifier];
    achievement.percentComplete = percentComplete;

    [GKAchievement reportAchievements:@[achievement] withCompletionHandler:completionHandler];
}

- (void)unlockAchievement:(NSString *)achievementIdentifier
{
    [self reportAchievement:achievementIdentifier withPercentComplete: 100 andCompletionHandler:^(NSError *error) {
        if (!_listener) {
            return;
        }

        if (error) {
            _listener->onAchievementUnlockError(achievementIdentifier.UTF8String, (int)error.code, error.localizedDescription.UTF8String);
        }
        else {
            _listener->onAchievementUnlocked(achievementIdentifier.UTF8String);
        }
    }];
}

- (void)setPercentComplete:(double)percentComplete forAchievement:(NSString *)achievementIdentifier
{
    [self reportAchievement:achievementIdentifier withPercentComplete: percentComplete andCompletionHandler:^(NSError *error) {
        if (!_listener) {
            return;
        }

        if (error) {
            _listener->onIncrementalAchievementStepError(achievementIdentifier.UTF8String, percentComplete, (int)error.code, error.localizedDescription.UTF8String);
        }
        else if (percentComplete == 100.0) {
            _listener->onIncrementalAchievementUnlocked(achievementIdentifier.UTF8String);
        }
        else {
            _listener->onIncrementalAchievementStep(achievementIdentifier.UTF8String, percentComplete);
        }
    }];
}

- (void)gameCenterViewControllerDidFinish:(nonnull GKGameCenterViewController *)gcViewController
{
    [self dismissViewController:gcViewController];
}

@end

NS_AX_BEGIN

void GameCenter::authenticatePlayer()
{
    [[GameCenterManager sharedGameCenterManager] authenticatePlayer];
}

void GameCenter::setListener(GameCenterListener* listener)
{
    [[GameCenterManager sharedGameCenterManager] setListener:listener];
}

void GameCenter::removeListener()
{
    [[GameCenterManager sharedGameCenterManager] removeListener];
}

void GameCenter::showLeaderboard(const std::string& leaderboardIdentifier)
{
    [[GameCenterManager sharedGameCenterManager] showLeaderboard: [NSString stringWithUTF8String:leaderboardIdentifier.c_str()]];
}

void GameCenter::showAllLeaderboards()
{
    [[GameCenterManager sharedGameCenterManager] showAllLeaderboards];
}

void GameCenter::showAchievements()
{
    [[GameCenterManager sharedGameCenterManager] showAchievements];
}

void GameCenter::getMyScore(const std::string& leaderboardIdentifier)
{
    [[GameCenterManager sharedGameCenterManager] getMyScoreFromLeaderboardWithIdentifier:[NSString stringWithUTF8String:leaderboardIdentifier.c_str()]];
}

void GameCenter::submitScore(const std::string& leaderboardIdentifier, long score)
{
    [[GameCenterManager sharedGameCenterManager] submitScore:score forLeaderboardIdentifier:[NSString stringWithUTF8String:leaderboardIdentifier.c_str()]];
}

void GameCenter::unlockAchievement(const std::string& achievementIdentifier)
{
    [[GameCenterManager sharedGameCenterManager] unlockAchievement:[NSString stringWithUTF8String:achievementIdentifier.c_str()]];
}

void GameCenter::setAchievementSteps(const std::string& achievementIdentifier, double steps)
{
    [[GameCenterManager sharedGameCenterManager] setPercentComplete:steps forAchievement:[NSString stringWithUTF8String:achievementIdentifier.c_str()]];
}

NS_AX_END
