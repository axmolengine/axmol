package org.axmol.lib;

import android.app.Activity;

import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.LeaderboardsClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;
import com.google.android.gms.games.leaderboard.LeaderboardScore;
import com.google.android.gms.games.leaderboard.LeaderboardScoreBuffer;
import com.google.android.gms.games.leaderboard.LeaderboardVariant;

public class GameServicesManager {

    private final Activity activity;
    private GamesSignInClient gamesSignInClient = null;
    private LeaderboardsClient leaderboardsClient = null;
    private AchievementsClient achievementsClient = null;

    GameServicesManager(Activity activity) {
        this.activity = activity;
        PlayGamesSdk.initialize(activity);
    }

    public void authenticatePlayer() {
        if (gamesSignInClient == null) {
            gamesSignInClient = PlayGames.getGamesSignInClient(activity);
            achievementsClient = PlayGames.getAchievementsClient(activity);
            leaderboardsClient = PlayGames.getLeaderboardsClient(activity);
        }

        gamesSignInClient.isAuthenticated().addOnCompleteListener(isAuthenticatedTask -> {
            boolean isAuthenticated = (isAuthenticatedTask.isSuccessful() && isAuthenticatedTask.getResult().isAuthenticated());
            if (isAuthenticated) {
                onConnectionStatusChanged(0);
            } else {
                onConnectionStatusChanged(2);
            }
        });
    }

    public void showLeaderboard(String leaderboardIdentifier) {
        leaderboardsClient.getLeaderboardIntent(leaderboardIdentifier).addOnSuccessListener(intent -> activity.startActivityForResult(intent, 9004));
    }

    public void showAllLeaderboards() {
        leaderboardsClient.getAllLeaderboardsIntent().addOnSuccessListener(intent -> activity.startActivityForResult(intent, 9004));
    }

    public void showAchievements() {
        achievementsClient.getAchievementsIntent().addOnSuccessListener(intent -> activity.startActivityForResult(intent, 9003));
    }

    public void getMyScore(String leaderboardIdentifier) {
        leaderboardsClient.loadPlayerCenteredScores(leaderboardIdentifier, LeaderboardVariant.TIME_SPAN_ALL_TIME, LeaderboardVariant.COLLECTION_PUBLIC, 1)
            .addOnSuccessListener(data -> {
                LeaderboardsClient.LeaderboardScores scores = data.get();
                if (scores == null) {
                    onMyScoreError(leaderboardIdentifier, 0, "LeaderboardScores is null");
                } else {
                    LeaderboardScoreBuffer buffer = scores.getScores();
                    LeaderboardScore score = buffer.getCount() > 0 ? buffer.get(0) : null;
                    if (score == null) {
                        onMyScoreError(leaderboardIdentifier, 0, "LeaderboardScore is null");
                    } else {
                        onMyScore(leaderboardIdentifier, score.getRawScore());
                    }
                }
            })
            .addOnFailureListener(exception -> onMyScoreError(leaderboardIdentifier, getStatusCode(exception), exception.getLocalizedMessage()));
    }

    public void submitScore(String leaderboardIdentifier, long score) {
        leaderboardsClient.submitScoreImmediate(leaderboardIdentifier, score)
            .addOnSuccessListener(data -> onScoreSubmitted(leaderboardIdentifier, score))
            .addOnFailureListener(exception -> onScoreSubmitError(leaderboardIdentifier, score, getStatusCode(exception), exception.getLocalizedMessage()));
    }

    public void unlockAchievement(String achievementIdentifier) {
        achievementsClient.unlockImmediate(achievementIdentifier)
            .addOnSuccessListener(data -> onAchievementUnlocked(achievementIdentifier))
            .addOnFailureListener(exception -> onAchievementUnlockError(achievementIdentifier, getStatusCode(exception), exception.getLocalizedMessage()));
    }

    public void setAchievementSteps(String achievementIdentifier, double steps) {
        achievementsClient.setStepsImmediate(achievementIdentifier, (int)steps)
            .addOnSuccessListener(isUnlocked -> {
                if (isUnlocked) {
                    onIncrementalAchievementUnlocked(achievementIdentifier);
                } else {
                    onIncrementalAchievementStep(achievementIdentifier, steps);
                }
            })
            .addOnFailureListener(exception -> onIncrementalAchievementStepError(achievementIdentifier, steps, getStatusCode(exception), exception.getLocalizedMessage()));
    }

    private int getStatusCode(Exception exception) {
        if (exception instanceof ApiException) {
            return ((ApiException)exception).getStatusCode();
        }
        return 0;
    }

    public native void onConnectionStatusChanged(int status);
    public native void onScoreSubmitted(String leaderboardIdentifier, long score);
    public native void onScoreSubmitError(String leaderboardIdentifier, long score, int errorCode, String errorDescription);
    public native void onMyScore(String leaderboardIdentifier, long score);
    public native void onMyScoreError(String leaderboardIdentifier, int errorCode, String errorDescription);
    public native void onIncrementalAchievementUnlocked(String achievementIdentifier);
    public native void onIncrementalAchievementStep(String achievementIdentifier, double steps);
    public native void onIncrementalAchievementStepError(String achievementIdentifier, double steps, int errorCode, String errorDescription);
    public native void onAchievementUnlocked(String achievementIdentifier);
    public native void onAchievementUnlockError(String achievementIdentifier, int errorCode, String errorDescription);

}
