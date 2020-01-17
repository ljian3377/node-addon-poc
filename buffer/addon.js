var addon = require('bindings')('addon');

function writeBuffer() {
  const length = 1024;
  const array = new Int32Array(length);
  array[0]  = 1;
  array[1]  = 2;
  for (let i = 2; i < length; i++) {
    array[i] = 1;
  }
  // console.log(array[length-1]);

  function done (result) {
    // console.log("done");
    console.log(result);
    // if (result instanceof ArrayBuffer) {
    //   console.log("returning buffer");
    //   const resultArray = new Int32Array(result);
    //   console.log(resultArray.length, resultArray[resultArray.length -1]);
    // }
    const used = process.memoryUsage();
    for (let key in used) {
      console.log(`Memory: ${key} ${Math.round(used[key] / 1024 / 1024 * 100) / 100} MB`);
    }
    writeBuffer();
  }
  addon.calculateAsync(array.buffer, done);
}

writeBuffer();
try {
  if (global.gc) { // --expose-gc
    console.log("gc ing");
    global.gc(true);
  }
} catch (e) {
  console.log("not able force gc");
}

console.log("sleep now");
setTimeout(() => {}, 60*60*1000);
