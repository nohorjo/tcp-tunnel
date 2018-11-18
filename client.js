const net = require('net');
const { argv } = require('yargs');

const remote = new net.Socket();
const local = new net.Socket();

remote.connect(8080, '127.0.0.1', () => {
    console.log('Connected to remote');
});

local.connect(argv.p, argv.h, () => {
    console.log('Connected to local');
});

remote.pipe(local);
local.pipe(remote);

local.on('end', () => {
    console.log('Local closed');
    remote.destroy();
});
