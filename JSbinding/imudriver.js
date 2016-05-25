var ffi = require('ffi');

var lib = ffi.Library('libwalkingdriver', {
    'initIMU': [ 'int', [] ],
    'getHeading': [ 'double', [] ],
    'getPitch': [ 'double', [] ],
    'getRoll': [ 'double', [] ],
    'setHeading': [ 'void', ['double'] ],
    'setPitch': [ 'void', ['double'] ],
    'setRoll': [ 'void', ['double'] ],
    'setHeadingRotationDirection': ['void', ['int']],
    'getHeadingRotationDirection': ['int', []]
});

module.exports = {
    init: lib.initIMU,
    heading: function (val) {
        if(typeof val === 'number' && isNaN(val))
            lib.setHeading(val);
        return Math.round(lib.getHeading()*100)/100;
    },
    pitch: function (val) {
        if(typeof val === 'number' && isNaN(val))
            lib.setPitch(val);
        return Math.round(lib.getPitch()*100)/100;
    },
    roll: function (val) {
        if(typeof val === 'number' && isNaN(val))
            lib.setRoll(val);
        return Math.round(lib.getRoll()*100)/100;
    },
    headingRotationDirection: function (val) {
        if(typeof val === 'boolean')
            lib.setHeadingRotationDirection(val ? 1 : 0);
        return lib.getHeadingRotationDirection() == 1;
    }
};
