const fs = require('fs')
const path = require('path')
const StreamWrapper = require('./stream-wrapper')

const FILE_TO_READ = 'test.in'
const FILE_TO_WRITE = 'test.out'

const inStream = fs.createReadStream(path.join(__dirname, FILE_TO_READ))
const transformStream = new StreamWrapper()
const outStream = fs.createWriteStream(path.join(__dirname, FILE_TO_WRITE))

inStream.pipe(transformStream).pipe(outStream)
