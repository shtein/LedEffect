

function isValidIP(ipaddress){  
  var regexp = /^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;  

  return regexp.test(ipaddress);
}

function isNotEmpty(value) {
  return value != "";
}

function fillFied(id, value, enable = true){
  elem = document.getElementById(id);
  if(elem){
    elem.disabled = !enable;
    switch(elem.tagName){
      case "SPAN":
        elem.textContent = value;
      break;
      case "INPUT":
        switch(elem.type.toUpperCase()){
          case "TEXT": 
            elem.value = value;
          break;
          case "CHECKBOX":
            elem.checked = value;
          break;
        }
      break;
    }
  }
}

function enableInputSetValue(id, value, enable){
  fillFied(id, enable ? value : "", enable);
}

function getValidValue(id, func = null, errMsg = ""){
  var elem = document.getElementById((id));
  var value = elem.value;
  value.trim();

  if(func && !func(elem.value) ){
    showError(errMsg);
    elem.focus();
    throw new Error(errMsg);
  }

  return value;  
}

function setupApiProgress(parentElem){
  //Container
  var divContainer = document.createElement("div");
  divContainer.className = "progress-container";
  if(parentElem)
    parentElem.appendChild(divContainer);
  
  //Progress
  var divProgress = document.createElement("div");
  divProgress.className = "progress";
  divProgress.id = "progress";
  divContainer.appendChild(divProgress);  
  

  apiCallProgress.callback = function(step){   
    divProgress.style.display = step == true ? "block" : "none";    
  }
}


function showBannerMessage(msg, isError){
  if(!this.banner){
    this.banner = document.createElement("div");
    this.banner.className = "banner";
    document.body.appendChild(this.banner);

    var spClose = document.createElement("div");
    spClose.className = "close";
    spClose.innerHTML = "&times;";
    spClose.onclick = function(){
      this.banner.style.display = "none";
    }.bind(this);
    
    this.banner.appendChild(spClose);
    
    this.message = document.createElement("span");
    this.message.className = "message";
    this.banner.appendChild(this.message);
  }

  this.banner.style.display = "block";
  this.banner.style.backgroundColor = isError?  "#f44336" : "#30c513"; 
  this.message.innerHTML = msg;
}

function showError(error){
  showBannerMessage(error, true);
}

function showMessage(msg){
  showBannerMessage(msg, false);
}
