url = document.location.href;
    params = url.split('?')[1].split('&');
    let dllData = {};
    let tmp;
    for (let i = 0, l = params.length; i < l; i++) {
         tmp = params[i].split('=');
         dllData[tmp[0]] = tmp[1];
    }
    let dllName = document.getElementById('dllname-' + dllData.dll_ID).innerHTML;
    let processes = document.getElementById("dll-" + dllData.dll_ID).innerHTML;
    let separatedProcesses = [...processes.split(",")];
    let totalProcessesNum=separatedProcesses.length;
    let dllString=totalProcessesNum+' Processes used: '+'<span class="itemTitle">'+ dllName+'</span>';
    let dlltitle=document.getElementById('dtitle');
    dlltitle.innerHTML = dllString;

    for (let i=0 ; i < totalProcessesNum; i++) {
        tr = $('<tr/>'); 
        let process = document.getElementById("process-" +(i + 1));       
        let processName = process.innerHTML;
        let sepProcessID=[...process.getAttribute('class').split(',')];
        tr.append("<td>" + (i+1) + "</td>");
        tr.append("<td>" + processName + "</td>");
        tr.append("<td>" + sepProcessID[0] + "</td>");
        sepProcessID[1] != undefined ? tr.append("<td>" + sepProcessID[1] + "</td>") : tr.append("<td>" + "" + "</td>");
        sepProcessID[2] != undefined ? tr.append("<td>" + sepProcessID[2] + "</td>") : tr.append("<td>" + "" + "</td>");
        $("#dlltable").append(tr);
    }
    