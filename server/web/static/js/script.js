let rnd1 = Math.floor(Math.random()*10)
let rnd2 = Math.floor(Math.random()*10)
let rnd3 = Math.floor(Math.random()*10)
let rnd4 = Math.floor(Math.random()*10)

let result =""
result+= `<div class="card">
            <div class="card-header">
                <h2 class="font-weight-bold">Your Authentication Code</h2>
            </div>
            <div class="card-body font-weight-bold">
                <h3>Code <br> ${rnd1} ${rnd2} ${rnd3} ${rnd4}</h3>
            </div>
            <div class="card-footer">
                <a href="index.html" class=" btn btn-primary text-center font-weight-bold text-white float-right" title="slot 1" role="button">Back</a>
            <div>
        </div>`
 document.getElementById("a").innerHTML=result;