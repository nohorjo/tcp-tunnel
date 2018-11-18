const net = require('net');
const { argv } = require('yargs');

const remote = new net.Socket();
const local = new net.Socket();

remote.connect(argv.r, argv.h, () => {
    console.log('Connected to remote');
});

local.connect(argv.p, '127.0.0.1', () => {
    console.log('Connected to local');
});

remote.pipe(local);
local.pipe(remote);

remote.on('end', () => {
    console.log('Remote closed');
});

local.on('end', () => {
    console.log('Local closed');
    remote.destroy();
});
