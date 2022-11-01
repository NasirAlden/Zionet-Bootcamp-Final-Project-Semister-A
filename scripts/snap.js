
    // window.onload = function () {
    url = document.location.href;
    params = url.split('?')[1].split('&');
    let data = {};
    let tmp;
    for (let i = 0, l = params.length; i < l; i++) {
         tmp = params[i].split('=');
         data[tmp[0]] = tmp[1];
    }
    let snap=document.getElementById('snaptitle');
    snap.innerHTML = document.getElementById('snap'+data.snapID).innerHTML;
    let tr;
    let pNames = [...document.querySelectorAll(`[id*=${'process-name-'+data.snapID}`)];
    let pids = [...document.querySelectorAll(`[id*=${'process-pid-'+data.snapID}`)];
    let pFaults = [...document.querySelectorAll(`[id*=${'process-pFault-'+data.snapID}`)];
    let wSizes = [...document.querySelectorAll(`[id*=${'process-wSize-'+data.snapID}`)];
    let qPools = [...document.querySelectorAll(`[id*=${'process-qPool-'+data.snapID}`)];
    let qPeaks = [...document.querySelectorAll(`[id*=${'process-qPeak-'+data.snapID}`)];
    let pFiles = [...document.querySelectorAll(`[id*=${'process-pFile-'+data.snapID}`)];
    let dllCounts = [...document.querySelectorAll(`[id*=${'process-dllCount-'+data.snapID}`)];
    let processCount=pids.length;

    let maxWSize = wSizes[0].innerHTML,
      minWSize = wSizes[0].innerHTML,
      maxWSizeIndex,
      minWSizeIndex,
      maxDlls = dllCounts[0].innerHTML,
      minDlls = dllCounts[0].innerHTML,
      maxDllsIndex,
      minDllsIndex;
    for (let i=0 ; i < processCount; i++) {
        if (Number(wSizes[i].innerHTML) > Number(maxWSize)) {
          maxWSize = Number(wSizes[i].innerHTML);
          maxWSizeIndex = i+1;
        }
        if (Number(wSizes[i].innerHTML) < Number(minWSize)) {
          minWSize = Number(wSizes[i].innerHTML);
          minWSizeIndex = i+1;
        }
        if (Number(dllCounts[i].innerHTML) > Number(maxDlls)) {
          maxDlls = Number(dllCounts[i].innerHTML);
          maxDllsIndex = i+1;
        }
        if (Number(dllCounts[i].innerHTML) < Number(minDlls)) {
          minDlls = Number(dllCounts[i].innerHTML);
          minDllsIndex = i+1;
        }
        tr = $('<tr/>');     
        tr.append("<td>" + '.'+(i+1) + "</td>");   
        tr.append("<td>" + pNames[i].innerHTML + "</td>");
        tr.append("<td>" + pids[i].innerHTML + "</td>");
        tr.append("<td>" + pFaults[i].innerHTML + "</td>");
        tr.append("<td>" + wSizes[i].innerHTML + "</td>");
        tr.append("<td>" + qPools[i].innerHTML + "</td>");
        tr.append("<td>" + qPeaks[i].innerHTML + "</td>");
        tr.append("<td>" + pFiles[i].innerHTML + "</td>");
        tr.append("<td>" + dllCounts[i].innerHTML + "</td>");
        tr.append("<td>"+
        "<div id=\"dlllist"+i+"\" class=\"form-group\">"+
        "</div>"+"</td>");     
        $('#snaptable').append(tr);
        let dllList = [...document.querySelectorAll(`[id*=${'dll-'+data.snapID+pids[i].innerHTML}`)];
        let op;
        op=$('<select/ style="width:87px">');  
        let j=0;
        for(;j<dllList.length;j++){ 
        op.append("<option value="+(j+1)+">"+dllList[j].innerHTML+"</option>"); 
        }
        op.append("<option value=\"\" selected disabled hidden>Dlls List</option>");
        $('#dlllist'+i).append(op);
      }
      console.log(maxWSize+"----"+ maxWSizeIndex);
      console.log(minWSize + "----" + minWSizeIndex);
      console.log(maxDlls + "----" + maxDllsIndex);
      console.log(minDlls + "----" + minDllsIndex);
    

let found = 1,
  found1,
  found2;
function searchTAG(tag, string) {
  found=1;
  let aTags = document.getElementsByTagName(tag);
  let searchText = string;
  for (let i = 0; i < aTags.length; i++) {
    if (aTags[i].textContent == searchText && found==2 ) {
      found2 = aTags[i];
      found=1;
      break;
    }
    if (aTags[i].textContent == searchText && found==1 ) {
      found1 = aTags[i];
      found=2;
      continue;
    }
    
  }
}


searchTAG("td",maxWSize);
found1.innerHTML =
  '<span style="color:red;font-weight:bold;">' +
  maxWSize +
  ' <img src="images/warning.png" alt="Max Memory Usage!" width="25px"></span>';

searchTAG("td", minWSize);
found1.innerHTML =
  '<span style="color:green;font-weight:bold;">' +
  minWSize +
  ' <img src="images/greenlike.png" width="32px" style="margin-top:5px;" alt="Max Memory Usage!"></span>';

searchTAG("td", maxDlls);
found1.innerHTML =
  '<span style="color:purple;font-weight:bold;font-size:20px;box-shadow: 0 0 5px 5px #ff7b00;">' +
  maxDlls +
  "</span>";

searchTAG("td", minDlls);
found1.innerHTML =
  '<span style="color:blue;font-weight:bold;font-size:20px;box-shadow: 0 0 5px 5px #00ffea;">' +
  minDlls +
  "</span>";

  //******************************* */
found2.innerHTML =
  '<span style="color:red;font-weight:bold;">' +
  maxWSize +
  ' <img src="images/warning.png" alt="Max Memory Usage!" width="25px"></span>';

found2.innerHTML =
  '<span style="color:green;font-weight:bold;">' +
  minWSize +
  ' <img src="images/greenlike.png" width="32px" style="margin-top:5px;" alt="Max Memory Usage!"></span>';

found2.innerHTML =
  '<span style="color:purple;font-weight:bold;font-size:20px;box-shadow: 0 0 5px 5px #ff7b00;">' +
  maxDlls +
  "</span>";

found2.innerHTML =
  '<span style="color:blue;font-weight:bold;font-size:20px;box-shadow: 0 0 5px 5px #00ffea;">' +
  minDlls +
  "</span>";