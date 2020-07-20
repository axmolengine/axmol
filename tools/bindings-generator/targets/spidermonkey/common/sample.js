var cc = cc || {}

cc.p = function (x, y) {
	var tmp = new Float32Array(2);
	tmp[0] = x;
	tmp[1] = y;
	return tmp;
}

cc.c3 = function (r, g, b) {
	var tmp = new Uint8Array(3);
	tmp[0] = r;
	tmp[1] = g;
	tmp[2] = b;
	return tmp;
}

cc.c4 = function (r, g, b, o) {
	var tmp = new Uint8Array(4);
	tmp[0] = r;
	tmp[1] = g;
	tmp[2] = b;
	tmp[3] = o;
	return tmp;
};

var randColorComp = function () {
	return Math.round(Math.random() * 255);
};

var director = cc.Director.sharedDirector();
var audioEngine = cc.SimpleAudioEngine.sharedEngine();
var winSize = director.getWinSize();

log("size: " + winSize[0] + "," + winSize[1]);

var scene = cc.Scene.create();
var layer = cc.LayerGradient.create(cc.c4(0, 0, 0, 255), cc.c4(0, 128, 255, 255));
scene.addChild(layer);

for (var i=0; i < 30; i++) {
	var sprite = cc.Sprite.create("Icon.png");
	var pos = cc.p(Math.random() * 480, Math.random() * 320);
	sprite.setPosition(pos);
	sprite.setColor(cc.c3(randColorComp(), randColorComp(), randColorComp()));
	layer.addChild(sprite);
}

var label = cc.LabelTTF.create("Testing Label TTF", "Marker Felt", 18.0);
label.setPosition(cc.p(240, 300));

layer.addChild(label);

audioEngine.setBackgroundMusicVolume(0.5);
audioEngine.playBackgroundMusic("bgmusic.mp3", true);

director.runWithScene(scene);
