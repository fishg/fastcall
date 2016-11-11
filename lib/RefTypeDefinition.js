'use strict';
const _ = require('lodash');
const assert = require('assert');
const verify = require('./verify');
const ref = require('./ref-libs/ref');
const Parser = require('./Parser');

class RefTypeDefinition {
    constructor(args) {
        verify(args);

        assert(_.isObject(args.library), '"library" is not an object.');
        verify(_.isString(args.propertyName), '"args.propertyName" is not a string.');
        this.library = args.library;
        this.propertyName = args.propertyName;
        this._FactoryType = args.FactoryType;

        if (_.isString(args.def)) {
            const parsed = this._parse(args.def);
            this.name = parsed.name;
            this._defBody = parsed.defBody;
            this._type = this._typeFactory();
        }
        else {
            this.name = _.keys(args.def)[0];
            assert(_.isString(this.name), '"def" is invalid.');
            this._defBody = args.def[this.name];

            if (_.isFunction(this._defBody)) {
                this._type = this._defBody;
            }
            else if (_.isObject(this._defBody) || _.isString(this._defBody)) {
                this._type = this._typeFactory();
            }
            else {
                assert(false, '"def" is invalid.');
            }
        }
        this._type[this.propertyName] = this;
    }

    get type() {
        return this._type;
    }

    getFactory() {
        const factory = value => this.makePtr(value);
        factory[this.propertyName] = this;
        factory.type = this.type;
        return factory;
    }

    makePtr(value) {
        const propName = this.propertyName;
        if (value) {
            if (value instanceof Buffer) {
                return value;
            }

            if (_.isPlainObject(value) || _.isArray(value) || _.isString(value)) {
                value = new this.type(value);
            }

            if (value.buffer instanceof Buffer) {
                value = value.buffer;
            }
            else if (_.isFunction(value.ref)) {
                value = value.ref();
            }

            if (!value.type) {
                value.type = this.type;
            }
            
            if (!value[propName]) {
                value[propName] = this;
            }

            return value;
        }
        else if (value === null) {
            return null;
        }
        throw new TypeError(`Cannot make ${ propName } from: ` + value);
    }

    _parse(str) {
        const parser = new Parser(this.library);
        return parser.parseFields(str, this.propertyName);
    }

    _typeFactory() {
        verify(_.isFunction(this._FactoryType), '"args.FactoryType" is not a function.');
        verify(this._defBody);
        if (_.isPlainObject(this._defBody)) {
            this._defBody = this._resolveStringTypes(this._defBody);
        }
        if (_.isString(this._defBody)) {
            this._defBody = this._resolveStringType(this._defBody);
        }
        return new this._FactoryType(this._defBody);
    }

    _resolveStringTypes(defObj) {
        const result = {};
        _.each(defObj, (value, key) => {
            let type = defObj[key];
            if (_.isString(type)) {
                type = this._resolveStringType(type);
            }
            result[key] = type;
        });
        return result;
    }

    _resolveStringType(type) {
        let match = /(\w+)\s*(?:\[\s*(\d+)\s*\])?/.exec(type);
        if (match) {
            type = match[1];
            let def = this.library.findRefDeclaration(type);
            if (def) {
                type = def.type;
                if (match[2]) {
                    type = def._makeTypeWithLength(match[2]);
                }
            }
        }
        return type;
    }

    _makeTypeWithLength(len) {
        len = _.isString(len) ? Number.parseInt(len) : len;
        let itemType = this._defBody;
        let FactoryType = this._FactoryType;
        assert(len > 0 && itemType && _.isFunction(FactoryType), 'Invalid array type definition.');
        return new FactoryType(itemType, len);
    }
}

module.exports = RefTypeDefinition;