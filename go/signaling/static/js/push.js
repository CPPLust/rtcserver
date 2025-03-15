'use strict';


var pushBtn = document.getElementById("pushBtn");

pushBtn.addEventListener("click", startPush);

var uid = $("#uid").val()
var streamName = $("#streamName").val()
var audio = $("#audio").val()
var video = $("#video").val()
var offer = "";
var pc;
const config = {};

function startPush() {
	console.log("send push:/signaling/push");


	$.post("/signaling/push",
		{"uid":uid, "streamName":streamName,"audio":audio,"video":video},
		function (data, textStatus){
			console.log("push response: " + JSON.stringify(data));
			if ("success" == textStatus && 0 == data.errNo) {
				$("#tips1").html("<font color='blue'>推流请求成功!</font>")
				console.log("remote answer: \r\n" + data.data.sdp)
        offer = data.data;
        pushStream();
			} else {
				$("#tips1").html("<font color='red'>推流请求失败!</font>")
			}
		},
		"json"
	);
}

function pushStream() {
    pc = new RTCPeerConnection(config);

    pc.setRemoteDescription(offer).then(
        setRemoteDescriptionSuccess,
        setRemoteDescriptionError
    );
}




function setRemoteDescriptionSuccess() {
    console.log("pc set remote description success");
    console.log("request screen share");
    window.postMessage({type: "SS_UI_REQUEST", text: "push"}, "*");
}
function setRemoteDescriptionError(error) {
    console.log("pc set remote description error: " + error);
}
