package main
 
import (
    "fmt"
    "net/http"
)


func startHttp(port string) {

}
 
func main() {
	staticUrl := "/static/"
	
	fs := http.FileServer(http.Dir("./static"))

	http.Handle(staticUrl, http.StripPrefix(staticUrl,fs))

	err := http.ListenAndServeTLS("10.10.40.66:8081", "./conf/www.testrtc.cn.pem", "./conf/www.testrtc.cn.key", nil)

	if err != nil {
		fmt.Println(err)
	}
}
 
