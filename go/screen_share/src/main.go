package main

import (
	"net/http"
	"fmt"
)
func main() {
	//1. 定义一个url前缀
	staticUrl := "/static/"

	//2. 定义一个filesever ./static 是相对可执行文件的目录下的static
	fs := http.FileServer(http.Dir("./static"))
	

	//3. 绑定url 和fileserver
	//http://www.testrtc.cn/static/screen_share.html
	//
	http.Handle(staticUrl, http.StripPrefix(staticUrl,fs))

	//4. 启动http server
	//err := http.ListenAndServe("10.10.40.66:8080", nil)
	err := http.ListenAndServeTLS("10.10.40.66:8080","./conf/www.testrtc.cn.pem","./conf/www.testrtc.cn.key", nil)
	if err != nil {
		fmt.Println(err)
	}

//	fmt.Println("vim-go")
}
