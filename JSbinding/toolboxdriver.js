var ffi = require('ffi');
var simpleCallback = ffi.Function('void', []);

var lib = ffi.Library('libwalkingdriver', {
    'initToolboxDriver': [ 'int', [] ],
    'getMotorPowerLevel': [ 'double', [] ],
    'getLogicPowerLevel': [ 'double', [] ],
    'getButton': [ 'int', ['int'] ],
    'getSensor': [ 'int', ['int'] ],
    'getCollisionDetector': [ 'int', ['int'] ],
    'setSensorsCallback': [ 'void', [simpleCallback] ],
    'enableSensorCallback': [ 'void', ['int'] ],
    'disableSensorCallback': [ 'void', ['int'] ],
    'setCollisionsCallback': [ 'void', [simpleCallback] ],
    'enableCollisionCallback': [ 'void', ['int'] ],
    'disableCollisionCallback': [ 'void', ['int'] ],
    'setPWM': [ 'void', ['int', 'uint8'] ],
    'setLED': [ 'void', ['int', 'int'] ]
});
var sensorsCallback;
var collisionsCallback;

module.exports = {
    init: lib.initToolboxDriver,
    motorPowerLevel: function () {
        return Math.round(lib.getMotorPowerLevel()*100)/100;
    },
    logicPowerLevel: function () {
        return Math.round(lib.getLogicPowerLevel()*100)/100;
    },
    button: function (number) {
        return lib.getButton(number) == 1;
    },
    sensor: function (number) {
        return lib.getSensor(number) == 1;
    },
    collision: function (number) {
        return lib.getCollisionDetector(number) == 1;
    },
    onCollision: function (callback) {
        collisionsCallback = callback;
        var cbck = ffi.Callback('void', [], collisionsCallback);
        lib.setCollisionsCallback(cbck);
    },
    onSensor: function (callback) {
        sensorsCallback = callback;
        var cbck = ffi.Callback('void', [], sensorsCallback);
        lib.setCollisionsCallback(cbck);
    },
    enableSensorCallback: lib.enableSensorCallback,
    disableSensorCallback: lib.disableSensorCallback,
    enableCollisionCallback: lib.enableCollisionCallback,
    disableCollisionCallback: lib.disableCollisionCallback,
    PWM: lib.setPWM,
    LED: lib.setLED
};
