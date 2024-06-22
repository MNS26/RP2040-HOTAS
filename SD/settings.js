var hexCol = ["0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"]
var hexRow = ["00:","10:","20:","30:","40:","50:","60:","70:","80:","90:","A0:","B0:","C0:","D0:","E0:","F0:"]
var usagePageList = {
  "Undefined": 0x00,
  "Generic Desktop Controls": 0x01,
  "Simulation Controls": 0x02,
  "VR Controls": 0x03,
  "Sport Controls": 0x04,
  "Game Controls": 0x05,
}
var usagePageSubList = {
  "Undefined": {
    "Undefined":0x00
  },
  "Generic Desktop Controls": {
    "Undefined": 0x00,
    "Pointer": 0x01,
    "Mouse": 0x02,
    "Joystick": 0x04,
    "Game Pad": 0x05,
    "keyboard": 0x06,
    "Multi-axis COntroller": 0x08
  },
  "Simulation Controls": {
    "Undefined" : 0x00,
    "Flight Simulation Device": 0x01,
    "Automobile Simulation Device": 0x02,
    "Tank Simulation Device": 0x03,
    "Spaceship Simulation Device": 0x04,
    "Submarine Simulation Device": 0x05, // OceanGate anyone?
    "Sailing Simulation Device": 0x06,
    "Motorcycle Simulation Device": 0x07,
    "Sports Simulation Device": 0x08,
    "Airplane Simulation Device": 0x09,
    "Helicopter Simulation Device": 0x0A,
    "Magic Carpet Simulation Device": 0x0B,
    "Bicycle Simulation Device": 0x0C
  }
}

// test

var usagePage = 0;
var usage = 0;
var axisCount=0;
var ButtonCount=0;
var HatCount=0;

function onBodyLoad() {
  console.log("we are loaded!!");
  setTimeout(getI2CList, 500);
  getHIDValues();
  onListSelect();
  populateUsagePage();
  document.getElementById("UsagePageType").addEventListener("change", UsagePageTypeChanged);
  document.getElementById("UsagePageSubType").addEventListener("change", UsagePageSubTypeChanged);
  //populateMarkers();
}


function populateMarkers() {
  var options = "";
  for (var i = 0; i <= 80; i++){
    options += '<option value='+i+'/>';
  }
  document.getElementById('ListCountAxis').innerHTML = options;
}

function onListSelect() {  
  var mylist = document.getElementById("myList");  
  document.getElementById("favourite").value = mylist.options[mylist.selectedIndex].text;  
}  

async function getI2CList() {
  const res = await fetch("listI2C?");
  var ids = await res.json();
  var tbodyRef = document.getElementById('I2CTable').getElementsByTagName('tbody')[0];
  // Clear the existing data
  tbodyRef.innerHTML = '';
  // Insert a row at the end of table
  var newRow = tbodyRef.insertRow();
  var newCell = newRow.insertCell();
  var newText = document.createTextNode("");
  newCell.appendChild(newText);
  var row = 0;
  for (var i = 0; i<16; i++) {
    var newCell = newRow.insertCell();
    var newText = document.createTextNode(hexCol[i]);
    newCell.appendChild(newText);
  }
  var newRow = tbodyRef.insertRow();
  var row = 0;
  for (var j = 0; j<128; j++) {
    if ((j % 16) == 0) {
      var newCell = newRow.insertCell();
      var newText = document.createTextNode(hexRow[row]);
      newCell.appendChild(newText);
    }
    var newCell = newRow.insertCell();
    if (ids.includes(j) == true) {
      if (j<7)
        var newText = document.createTextNode(' ');
      else
        var newText = document.createTextNode('X');
    } else {
      if (j<7)
        var newText = document.createTextNode(' ');
      else
        var newText = document.createTextNode('-');
    }
    newCell.appendChild(newText);
    // Insert a row at the end of table when we have 16 items
    if ( (j+1)%16 == 0) {
      var newRow = tbodyRef.insertRow();
      row++;
    }
  }
  setTimeout(getI2CList, 500);
}

function updateAxis() { 
//  fetch("settings/updatehid?axis="+ document.getElementById("axiscount").value);
}

function updateButtons() { 
//  fetch("settings/updatehid?buttons="+ document.getElementById("buttoncount").value);
}

function updateHats() { 
//  fetch("settings/updatehid?hats="+ document.getElementById("hatcount").value);
}

async function getHIDValues() {
  //const current_state = await fetch("settings/updatehid?getValues=");
  //var values = await current_state.json();
  var response;

  response = await fetch("settings/updatehid?usagepage=");
  //document.getElementById("axiscount").value = response.text();  
  response = await fetch("settings/updatehid?usage=");
  //document.getElementById("axiscount").value = response.text();  

  response = await fetch("settings/updatehid?axis=");
  document.getElementById("axiscount").value = await response.text(); // values[0];  
  response = await fetch("settings/updatehid?buttons=");
  document.getElementById("buttoncount").value = await response.text(); //values[1];  
  response = await fetch("settings/updatehid?hats=");
  document.getElementById("hatcount").value = await response.text(); //values[2];  
}

async function updateHID() {
  // fetch("settings/updatehid?"+
  //       "usagepage="+ usagePage+
  //       "&usage="+ usage+
  //       "&axis="+ document.getElementById("axiscount").value+
  //       "&buttons="+ document.getElementById("buttoncount").value+
  //       "&hats="+ document.getElementById("hatcount").value
  // );
  await fetch("settings/updatehid?usagepage="+ usagePage);
  await fetch("settings/updatehid?usage="+ usage);
  await fetch("settings/updatehid?axis="+ document.getElementById("axiscount").value);
  await fetch("settings/updatehid?hats="+ document.getElementById("hatcount").value);
  await fetch("settings/updatehid?buttons="+ document.getElementById("buttoncount").value);
  await fetch("settings/updatehid?restart=");
}

function populateUsagePage() {
  var usageListPageType = document.getElementById("UsagePageType");
  usageListPageType.innerHTML = '';
  console.log(usagePageList);

  for (const [k,v] of Object.entries(usagePageList)) {
    console.log(k + "-" + v);
    usageListPageType.add( new Option(k, v));
  }
}

function UsagePageTypeChanged(e) {
  var index = e.target.options.selectedIndex;
  console.log(index);
  usagePage = index;
  var key = e.target.options[index].innerText;
  console.log(key);
  var sublist = usagePageSubList[key];
  console.log(sublist);
  var UsagePageSubType = document.getElementById("UsagePageSubType");
  UsagePageSubType.innerHTML = '';
  if (sublist) {
    for (const [k,v] of Object.entries(sublist)) {
      UsagePageSubType.add( new Option(k, v));
    }
  } else {
    UsagePageSubType.add( new Option("unsupported"));
  }
}

function UsagePageSubTypeChanged(e) {
  usage = e.target.options.selectedIndex;
  console.log(usagePage, usage);
}
