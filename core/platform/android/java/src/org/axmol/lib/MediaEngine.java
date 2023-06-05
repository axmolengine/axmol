package org.axmol.lib;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Point;
import android.media.MediaCrypto;
import android.media.MediaFormat;
import android.net.Uri;
import android.os.Handler;
import android.provider.MediaStore;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;

import com.google.android.exoplayer2.DefaultRenderersFactory;
import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.Format;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.Player;
import com.google.android.exoplayer2.Renderer;
import com.google.android.exoplayer2.mediacodec.MediaCodecAdapter;
import com.google.android.exoplayer2.mediacodec.MediaCodecInfo;
import com.google.android.exoplayer2.mediacodec.MediaCodecSelector;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.ProgressiveMediaSource;
import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DefaultDataSource;
import com.google.android.exoplayer2.video.MediaCodecVideoRenderer;
import com.google.android.exoplayer2.video.VideoFrameMetadataListener;
import com.google.android.exoplayer2.video.VideoRendererEventListener;
import com.google.android.exoplayer2.video.VideoSize;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.security.MessageDigest;
import java.util.ArrayList;

import android.media.MediaCodecInfo.CodecCapabilities;

public class MediaEngine implements Player.Listener {
    // The native media events, match with MEMediaEventType
    public static final int EVENT_PLAYING = 0;
    public static final int EVENT_PAUSED = 1;
    public static final int EVENT_STOPPED = 2;
    public static final int EVENT_COMPLETED = 3;
    public static final int EVENT_ERROR = 4;

    /** Media has been closed and cannot be played again. */
    public static final int STATE_CLOSED = 0;

    public static final int STATE_CLOSING = 1;

    /** Unrecoverable error occurred during playback. */
    public static final int STATE_ERROR = 2;

    /** Playback has been paused, but can be resumed. */
    public static final int STATE_PAUSED = 3;

    /** Media is currently playing. */
    public static final int STATE_PLAYING = 4;

    /** Media is being prepared for playback. */
    public static final int STATE_PREPARING = 5;

    /** Playback has been stopped, but can be restarted. */
    public static final int STATE_STOPPED = 6;

    /** Playback has been completed, but can be restarted. */
    public static final int STATE_COMPLETED = 7;


    // The desired video pixel format
    /**
     * COLOR_FormatYUV420SemiPlanar (yyyyyyyy uvuv)  (NV12)
     * COLOR_FormatYUV420Planar (yyyyyyyy uu vv)     (YUV420p)
     * COLOR_FormatYUV422SemiPlanar (Y0 U0 Y1 V0)    (YUY2)
     */
    public static final int DESIRED_PIXEL_FORMAT = CodecCapabilities.COLOR_FormatYUV420SemiPlanar; // desired pixel format: NV12

    public static final String TAG = "MediaEngine";

    private ExoPlayer mPlayer;
    private ByteBufferMediaCodecVideoRenderer mVideoRenderer;
    private ByteBuffer mLastVideoFrameData = null;
    private boolean mAutoPlay = false;
    private boolean mLooping = false;

    private boolean mIsH256 = false;

    private long mNativeObj = 0; // native object address for send event to C++, weak ref

    private int mState = STATE_CLOSED;

    Point mOutputDim = new Point(); // The output dim match with buffer
    Point mVideoDim = new Point(); // The video dim (validate image dim)

    /** ------ native methods ------- */
    public static native void nativeHandleEvent(long nativeObj, int arg1);
    public static native void nativeHandleVideoSample(long nativeObj, ByteBuffer sampleData, int sampleLen, int outputX, int outputY, int videoX, int videoY);

    public static Object CreateMediaEngine() {
        return new MediaEngine();
    }

    public void BindNativeObject(long nativeObj) {
        mNativeObj = nativeObj;
    }

    public void SetAutoPlay(boolean bAutoPlay)
    {
        mAutoPlay = bAutoPlay;
    }

    /**
     *
     * @param sourceUri:
     *                 url: http(s)://
     *                 disk: file://path/to/file
     *                 assets: file:///android_asset
     * @return
     */
    public boolean Open(String sourceUri)
    {
        Close();

        try {
            mState = STATE_PREPARING;

            final Context activity = AxmolEngine.getActivity();
            Uri uri = Uri.parse(sourceUri);
            DataSource.Factory dataSourceFactory = new DefaultDataSource.Factory(activity);
            MediaSource mediaSource =
                new ProgressiveMediaSource.Factory(dataSourceFactory)
                    .createMediaSource(MediaItem.fromUri(uri));
            Context context = activity.getApplicationContext();
            mPlayer = new ExoPlayer.Builder(context, new ByteBufferRenderersFactory(context)).build();
            mPlayer.addListener(this);

            final MediaEngine outputHandler = this;
            AxmolEngine.getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        mVideoRenderer = (ByteBufferMediaCodecVideoRenderer) mPlayer.getRenderer(0); // the first must be video renderer
                        mVideoRenderer.setOutputHandler(outputHandler);
                        mPlayer.setMediaSource(mediaSource);
                        mPlayer.prepare();
                        mPlayer.setRepeatMode(mLooping ? Player.REPEAT_MODE_ALL : Player.REPEAT_MODE_OFF);
                        mPlayer.setPlayWhenReady(mAutoPlay);
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                }
            });
        } catch (Exception ex) {
            ex.printStackTrace();
            return false;
        }
        return true;
    }
    public boolean Close() {
        if(mPlayer != null) {
            mState = STATE_CLOSING;
            final ExoPlayer player = mPlayer;
            mPlayer = null;
            final MediaEngine thiz = this;
            AxmolEngine.getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    player.removeListener(thiz);
                    player.stop();
                    player.release();
                    mState = STATE_CLOSED;
                }
            });
        }
        return true;
    }
    public boolean SetLoop(boolean bLooping) {
        if (mLooping != bLooping) {
            mLooping = bLooping;
            if (mPlayer == null) return false;
            AxmolEngine.getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (mPlayer != null)
                        mPlayer.setRepeatMode(mLooping ? Player.REPEAT_MODE_ALL : Player.REPEAT_MODE_OFF);
                }
            });
        }
        return true;
    }
    public boolean SetRate(double fRate) {
        if(mPlayer == null) return false;
        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null)
                    mPlayer.setPlaybackSpeed((float)fRate);
            }
        });
        return true;
    }
    public boolean SetCurrentTime(double fSeekTimeInSec)
    {
        if(mPlayer == null) return false;

        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null)
                    mPlayer.seekTo((long)(fSeekTimeInSec * 1000));
            }
        });
        return true;
    }
    public boolean Play() {
        if (mPlayer == null) return false;
        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null) {
                    if (mState == STATE_STOPPED) // TO-CHECK: can't reply after playback stopped
                        mPlayer.seekTo(0);
                    mPlayer.play();
                }
            }
        });
        return true;
    }
    public boolean Pause() {
        if(mPlayer == null) return false;
        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null)
                    mPlayer.pause();
            }
        });
        return true;
    }
    public boolean Stop() {
        if(mPlayer == null) return false;
        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null)
                    mPlayer.stop();
            }
        });
        return true;
    }

    /**
     * Get playback State
     *
     * @return
     *     int STATE_IDLE = 1;
     *     int STATE_BUFFERING = 2;
     *     int STATE_READY = 3;
     *     int STATE_ENDED = 4;
     */
    public int GetState() {
        return mState;
    }

    /** update video informations */
    private MediaFormat updateVideoInfo() {
        MediaFormat format = mVideoRenderer.getOutputMediaFormat();
        mOutputDim.x = format.getInteger(MediaFormat.KEY_WIDTH);
        if (format.containsKey(MediaFormat.KEY_CROP_LEFT)
            && format.containsKey(MediaFormat.KEY_CROP_RIGHT)) {
            mVideoDim.x = format.getInteger(MediaFormat.KEY_CROP_RIGHT) + 1
                - format.getInteger(MediaFormat.KEY_CROP_LEFT);
        }
        else
            mVideoDim.x = mOutputDim.x;

        mOutputDim.y = format.getInteger(MediaFormat.KEY_HEIGHT);
        if (format.containsKey(MediaFormat.KEY_CROP_TOP)
            && format.containsKey(MediaFormat.KEY_CROP_BOTTOM)) {
            mVideoDim.y = format.getInteger(MediaFormat.KEY_CROP_BOTTOM) + 1
                - format.getInteger(MediaFormat.KEY_CROP_TOP);
        }
        else
            mVideoDim.y = mOutputDim.y;

        mIsH256 = mVideoRenderer.isH256();

        mState = STATE_PLAYING;

        return format;
    }

    /** handler or listener methods */

    public void handleVideoSample(MediaCodecAdapter codec, int index) {
//        MediaFormat format = updateVideoInfo();

//        String mimeType = format.getString(MediaFormat.KEY_MIME); // "video/raw" (NV12)
//        Integer colorFormat = format.getInteger(MediaFormat.KEY_COLOR_FORMAT);
//        boolean NV12 = colorFormat == DESIRED_PIXEL_FORMAT;

        mState = STATE_PLAYING;
        updateVideoInfo();

        ByteBuffer tmpBuffer = codec.getOutputBuffer(index);
        nativeHandleVideoSample(mNativeObj, tmpBuffer, tmpBuffer.remaining(), mOutputDim.x, mOutputDim.y, mVideoDim.x, mVideoDim.y);
    }

    public void nativeEvent(int event){
        if(mNativeObj != 0) {
            nativeHandleEvent(mNativeObj, event);
        }
    }

    @Override
    public void onRenderedFirstFrame() {
        Player.Listener.super.onRenderedFirstFrame();
    }

    @Override
    public void onEvents(Player player, Player.Events events) {
        if (mPlayer == null) return;

        if(events.contains(Player.EVENT_IS_PLAYING_CHANGED)) {
            Log.d(TAG, "onIsPlayingChanged: " + mPlayer.isPlaying());
            if (mPlayer.isPlaying())
                nativeEvent(EVENT_PLAYING);
            else {
                nativeEvent(EVENT_PAUSED);
                mState = STATE_PAUSED;
            }
        }
        if(events.contains(Player.EVENT_PLAYBACK_STATE_CHANGED)) {
            int state = mPlayer.getPlaybackState();
            Log.d(TAG, "onPlaybackStateChanged: " + state);
            switch (state) {
                case Player.STATE_READY:
//                    mIsH256 = mVideoRenderer.isH2565();
                    break;
                case Player.STATE_ENDED:
                    nativeEvent(EVENT_COMPLETED);
                    mState = STATE_COMPLETED;
                    break;
                case Player.STATE_IDLE:
                    nativeEvent(EVENT_STOPPED);
                    mState = STATE_STOPPED;
                    break;
            }
        }
        if(events.contains(Player.EVENT_PLAYER_ERROR)) {
            Log.e(TAG, "onPlayerError: " + mPlayer.isPlaying());
            nativeEvent(EVENT_ERROR);
            mState = STATE_ERROR;
        }
        if(events.contains(Player.EVENT_VIDEO_SIZE_CHANGED)) {
            final VideoSize videoSize = mPlayer.getVideoSize();
            Log.d(TAG, String.format("onVideoSizeChanged: %d, %d", videoSize.width, videoSize.height));
        }
        if(events.contains(Player.EVENT_IS_LOADING_CHANGED)) {
            Log.d(TAG, "onIsLoadingChanged: " + mPlayer.isLoading());
        }
    }

    /** internal classes */
    class ByteBufferMediaCodecVideoRenderer extends MediaCodecVideoRenderer {

        MediaEngine mOutputHandler;

        public void setOutputHandler(MediaEngine handler) {
            this.mOutputHandler = handler;
        }

        public boolean isH256() {
            MediaCodecInfo codecInfo = getCodecInfo();
            return codecInfo.mimeType == "video/hevc";
        }

        public MediaFormat getOutputMediaFormat() {
            return getCodecOutputMediaFormat();
        }

        @TargetApi(17)
        @Override
        protected MediaCodecAdapter.Configuration getMediaCodecConfiguration(
            MediaCodecInfo codecInfo,
            Format format,
            MediaCrypto crypto,
            float codecOperatingRate) {
            MediaCodecAdapter.Configuration configuration = super.getMediaCodecConfiguration(codecInfo, format, crypto, codecOperatingRate);
            configuration.mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaEngine.DESIRED_PIXEL_FORMAT);

            return configuration;
        }

        public ByteBufferMediaCodecVideoRenderer(
            Context context,
            MediaCodecAdapter.Factory codecAdapterFactory,
            MediaCodecSelector mediaCodecSelector,
            long allowedJoiningTimeMs,
            boolean enableDecoderFallback,
            @Nullable Handler eventHandler,
            @Nullable VideoRendererEventListener eventListener,
            int maxDroppedFramesToNotify) {
            super(
                context,
                codecAdapterFactory,
                mediaCodecSelector,
                allowedJoiningTimeMs,
                enableDecoderFallback,
                eventHandler,
                eventListener,
                maxDroppedFramesToNotify);
            this.mByteBufferMode = true;
        }

        @RequiresApi(21)
        @Override
        protected void renderOutputBufferV21(
            MediaCodecAdapter codec, int index, long presentationTimeUs, long releaseTimeNs)
        {
            mOutputHandler.handleVideoSample(codec, index);
            super.renderOutputBufferV21(codec, index, presentationTimeUs, releaseTimeNs);
        }
        @Override
        protected void renderOutputBuffer(MediaCodecAdapter codec, int index, long presentationTimeUs) {
            mOutputHandler.handleVideoSample(codec, index);
            super.renderOutputBuffer(codec, index, presentationTimeUs);
        }
    }

    final class ByteBufferRenderersFactory extends DefaultRenderersFactory {

        public ByteBufferRenderersFactory(Context context) {
            super(context);
            setAllowedVideoJoiningTimeMs(0);
        }

        @Override
        protected void buildVideoRenderers(
            Context context,
            @ExtensionRendererMode int extensionRendererMode,
            MediaCodecSelector mediaCodecSelector,
            boolean enableDecoderFallback,
            Handler eventHandler,
            VideoRendererEventListener eventListener,
            long allowedVideoJoiningTimeMs,
            ArrayList<Renderer> out) {
            out.add(
                new ByteBufferMediaCodecVideoRenderer(
                    context,
                    getCodecAdapterFactory(),
                    mediaCodecSelector,
                    allowedVideoJoiningTimeMs,
                    enableDecoderFallback,
                    eventHandler,
                    eventListener,
                    MAX_DROPPED_VIDEO_FRAME_COUNT_TO_NOTIFY));
        }
    }
}


