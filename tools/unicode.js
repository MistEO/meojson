/* 
 * Unicode Block Generator
 *
 * Usage: 
 * `cd tools`
 * `npm i @unicode/unicode-10.0.0 regenerate --save`
 * `node unicode.js`
 */

const fs = require('fs')
const path = require('path')
const regenerate = require('regenerate')

const space_separator = regenerate()
    .add(require('@unicode/unicode-10.0.0/General_Category/Space_Separator/code-points'))
    .remove('\t', '\v', '\f', ' ', '\u00A0', '\uFEFF')

const id_start = regenerate()
    .add(require('@unicode/unicode-10.0.0/General_Category/Uppercase_Letter/code-points'))
    .add(require('@unicode/unicode-10.0.0/General_Category/Lowercase_Letter/code-points'))
    .add(require('@unicode/unicode-10.0.0/General_Category/Titlecase_Letter/code-points'))
    .add(require('@unicode/unicode-10.0.0/General_Category/Modifier_Letter/code-points'))
    .add(require('@unicode/unicode-10.0.0/General_Category/Other_Letter/code-points'))
    .add(require('@unicode/unicode-10.0.0/General_Category/Letter_Number/code-points'))
    .add(require('@unicode/unicode-10.0.0/Block/Emoticons/code-points'))
    .remove('$', '_')
    .removeRange('A', 'Z')
    .removeRange('a', 'z')

const id_continue = regenerate()
    .add(id_start)
    .add(require('@unicode/unicode-10.0.0/General_Category/Nonspacing_Mark/code-points'))
    .add(require('@unicode/unicode-10.0.0/General_Category/Spacing_Mark/code-points'))
    .add(require('@unicode/unicode-10.0.0/General_Category/Decimal_Number/code-points'))
    .add(require('@unicode/unicode-10.0.0/General_Category/Connector_Punctuation/code-points'))
    .remove('$', '_')
    .removeRange('0', '9')
    .removeRange('A', 'Z')
    .removeRange('a', 'z')

const outPath = path.join('../include', 'unicode.h')

const data = {
    space_separator,
    id_start,
    id_continue,
}

let hpp = '// This is a generated file. Do not edit.\n'
hpp += '#pragma once\n'

hpp += Object.keys(data).map(key => `
#define ${key.toUpperCase()} \\
    {${data[key].toArray().map(v => '0x' + Number(v).toString(16)).join(', ')}}

`).join('')

fs.writeFileSync(outPath, hpp)
