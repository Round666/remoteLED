var express = require('express');
var router = express.Router();
var {sendUdpMsg} = require('../tools/udp')

/* GET home page. */
router.get('/switch', function(req, res, next) {
  // 获取参数命令
  var command = req.query['command']
  // 发送upd消息  on=开灯  off=关灯
  sendUdpMsg(command,123,'192.168.4.1')
  res.send('success')
});

module.exports = router;
