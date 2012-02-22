var clutter = require('../build/Release/clutter');

if (clutter.init() != clutter.INIT_SUCCESS) {
	console.log("Failed to initialize clutter.");
	process.exit();
}

/* Create a new stage */
var stage = new clutter.Stage();
stage.setTitle('Text');
stage.resize(500, 500);
stage.setColor(0, 0, 0, 255);
stage.show();

/* Normal to load (default: sync) */
var texture1 = new clutter.Texture;
texture1.loadFile('fred.jpg');
texture1.setPosition(100, 100);
stage.add(texture1);

/* Asynchronize */
var texture2 = new clutter.Texture;
texture2.setLoadAsync(true);
texture2.loadFile('fred.jpg');
texture2.setPosition(200, 200);
stage.add(texture2);

console.log('texture2 getLoadAsync() = ' + texture2.getLoadAsync());

/* Keep aspect ratio */
var texture3 = new clutter.Texture;
texture3.keepAspectRatio(true);
texture3.loadFile('fred.jpg');
texture3.setWidth(50);
texture3.setPosition(50, 50);
stage.add(texture3);

console.log('texture3 keepAspectRatio() = ' + texture3.keepAspectRatio());

clutter.main();