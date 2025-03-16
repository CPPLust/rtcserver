'use strict';

var localVideo = document.getElementById("localVideo");
var pushBtn = document.getElementById("pushBtn");

pushBtn.addEventListener("click", startPush);

var uid = $("#uid").val();
var streamName = $("#streamName").val();
var audio = $("#audio").val();
var video = $("#video").val();
var offer = "";
var pc;
const config = {};
var localStream;

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

window.addEventListener("message", function(event) {
    if (event.origin != window.location.origin) {
        return;
    }

    if (event.data.type) {
        if (event.data.type == "SS_DIALOG_SUCCESS") {
            console.log("用户同意屏幕共享, streamId: " + event.data.streamId);
            startScreenStreamFrom(event.data.streamId);
        } else if (event.data.type == "SS_DIALOG_CANCEL") {
            console.log("用户取消屏幕共享");
        }
    }
});

function startScreenStreamFrom(streamId) {
    var constraints = {
        audio: false,
        video: {
            mandatory: {
                chromeMediaSource: "desktop",
                chromeMediaSourceId: streamId,
                maxWidth: window.screen.width,
                maxHeight: window.screen.height
            }
        }
    };

    navigator.mediaDevices.getUserMedia(constraints).then(
        handleSuccess).catch(handleError);
}
function handleSuccess(stream) {
    navigator.mediaDevices.getUserMedia({audio: true}).then(
        function(audioStream) {
            stream.addTrack(audioStream.getAudioTracks()[0]);
            localVideo.srcObject = stream;
            localStream = stream;
            pc.addStream(stream);
            pc.createAnswer().then(
                createSessionDescriptionSuccess,
                createSessionDescriptionError               
            );
        }
    ).catch(handleError);
}
function handleError(error) {
    console.log("get user media error: " + error);
}
function setRemoteDescriptionSuccess() {
    console.log("pc set remote description success");
    console.log("request screen share");
    window.postMessage({type: "SS_UI_REQUEST", text: "push"}, "*");
}
function createSessionDescriptionSuccess(answer) {
    console.log("answer sdp: \n" + answer.sdp);
}
function setRemoteDescriptionError(error) {
    console.log("pc set remote description error: " + error);
}
function createSessionDescriptionError(error) {
    console.log("pc create answer error: " + error);
}
