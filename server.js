const net = require('net');

const server = net.createServer(socket => {
    const tunnel = net.createServer(incoming => {
        incoming.pipe(socket);
        socket.pipe(incoming);
    });
    tunnel.listen(0, () => console.log('PORT', tunnel.address().port));
});

server.listen(process.env.PORT, () => {
    console.log('Started');
});
