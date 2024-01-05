const dgram = require('dgram')
// udp 对象
const udpClient = dgram.createSocket('udp4')
// 接受消息
udpClient.on("message", (msg, rinfo) => {
    console.log("接收来自：" + rinfo.address + ":" + rinfo.port + "的消息：" + msg.toString());
});
// 错误处理
udpClient.on("error", (err) => {
    console.error("客户端错误：" + err.message);
});


/**
 * 发送udp消息
 * @param {*} msg   消息体
 * @param {*} port  接收端口
 * @param {*} ip    接收ip
 */
function sendUdpMsg(msg,port,ip) {
    udpClient.send(msg,port,ip,(err)=>{
        if(err){
            udpClient.close()
        }
    })
}

module.exports = {
    sendUdpMsg
}
