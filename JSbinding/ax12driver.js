var ffi = require('ffi');
var simpleCallback = ffi.Function('void', []);

var lib = ffi.Library('libwalkingdriver', {
    'axGetPosition': [ 'int', ['int'] ],
    'axHasFinishedMove': [ 'int', [] ],
    'axIsForcing': [ 'int', [] ],
    'axSetTorqueSpeed': [ 'void', ['int', 'int', 'int', 'int'] ],
    'axMove': [ 'void', ['int', 'int', simpleCallback] ],
    'axReset': [ 'void', [] ],
});
var sensorsCallback;
var collisionsCallback;

module.exports = {
    position: function (id) {
        return lib.axGetPosition(id);
    },
    finishedMove: function () {
        return lib.axHasFinishedMove() !== 0;
    },
    isForcing: function () {
        return lib.axIsForcing() !== 0;
    },
    setTorqueSpeed: function (id, torque, speed, mode) {
        lib.axHasFinishedMove(id, torque, speed, mode);
    },
    move: function (id, position, callback) {
        lib.axHasFinishedMove(id, position, callback);
    },
    reset: lib.axReset
};
