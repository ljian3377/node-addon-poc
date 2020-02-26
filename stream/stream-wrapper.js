const { Transformer } = require('bindings')('transformer')
const { Transform } = require('readable-stream')

class StreamWrapper extends Transform {
    constructor( opts = {}) {
        super(opts);
        this._transformer = new Transformer();
    }

    _transform(chunk, encoding, cb) {
        this.push(this._transformer.trans(chunk));
        cb();
    }
}

module.exports = StreamWrapper;
