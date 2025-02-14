package main

import (
	"flag"
	"fmt"
	"signaling/src/framework"
)


func main() {
	flag.Parse()

	fmt.Println("main......")
	err := framework.Init("./conf/framework.conf")
	if err != nil {
		panic(err)
	}
	//glog.Info("hello main..")

	//静态资源处理 /static
	framework.RegisterStaticUrl()

	//start http server
	go startHttp()

	//start https server
	startHttps()
}

func startHttp() {
	err := framework.StartHttp()
	if err != nil {
		panic(err)
	}
}


func startHttps() {
	err := framework.StartHttps()
	if err != nil {
		panic(err)
	}
}
