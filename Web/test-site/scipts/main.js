/*
let myHeading = document.querySelector("h1")
myHeading.textContent = "Hello world!"

let myVariable = "小明";

document.querySelector("html").addEventListener("click",function(){
    alert("你好")
});
*/


let myImg = document.querySelector("img");
myImg.onclick = function () {
    let mySrc = myImg.getAttribute("src");
    if (mySrc === "https://pss.bdstatic.com/static/superman/img/logo/logo_white-d0c9fe2af5.png") {
        myImg.setAttribute("src", "https://www.google.com/images/branding/googlelogo/1x/googlelogo_color_272x92dp.png");
    } else {
        myImg.setAttribute("src", "https://pss.bdstatic.com/static/superman/img/logo/logo_white-d0c9fe2af5.png");
    }
};


let mybutton = document.querySelector("button");
let myHeading = document.querySelector("h1");

function setUserName() {
    let myName = prompt("请输入你的名字。");
    if (!myName) {
        setUserName();
    } else {
        localStorage.setItem("name", myName);
        myHeading.textContent = "你好" + myName;
    }

};

if (!localStorage.getItem("name")) {
    setUserName();
} else {
    let localStorage = localStorage.getItem("name");
    myHeading.textContent = "你好" + myName;
}

mybutton.onclick = function () {
    setUserName();
}



