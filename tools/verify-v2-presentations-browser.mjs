import { chromium } from "playwright";
import assert from "node:assert/strict";

const browser=await chromium.launch({headless:true});
const page=await browser.newPage({viewport:{width:390,height:844}});
const pageErrors=[];
page.on("pageerror",e=>pageErrors.push(String(e)));

await page.goto("http://127.0.0.1:8000/index.html",{waitUntil:"networkidle"});
await page.waitForFunction(()=>document.querySelector("#eventNote")?.textContent.includes("演出システム"));
assert.equal(pageErrors.length,0,pageErrors.join("\n"));

async function setFlag(label){
  const d=page.locator("#v2Debug");
  if(!(await d.evaluate(el=>el.open))) await page.locator("#v2Debug > summary").click();
  await page.selectOption("#flagPicker",{label});
  await page.click("#applyFlag");
  await page.waitForFunction(()=>document.querySelector("#flagStatus")?.textContent.includes("C++に登録"));
  if(await d.evaluate(el=>el.open)) await page.locator("#v2Debug > summary").click();
}
async function stopAll(){
  for(let i=0;i<3;i++){
    const b=page.locator('.stop[data-stop="'+i+'"]');
    await b.waitFor({state:"visible"});
    await page.waitForFunction((idx)=>!document.querySelector('.stop[data-stop="'+idx+'"]')?.disabled,i);
    await b.click();
  }
}
async function reset(){
  await page.click("#reset");
  await page.waitForFunction(()=>!document.querySelector("#lever")?.disabled);
}

// 1. Full freeze chain.
await setFlag("フリーズ");
await page.click("#lever");
await page.waitForTimeout(120);
assert(await page.locator(".machine").evaluate(el=>el.classList.contains("freeze-all-off")),"freeze blackout");
for(let i=1;i<=3;i++)assert(await page.locator("#reel"+i).evaluate(el=>el.classList.contains("reverse-spinning")),"reverse reel "+i);
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="PREMIUM FREEZE");
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="🟦7を狙え");
await page.waitForFunction(()=>[...document.querySelectorAll(".stop")].every(b=>!b.disabled));
await stopAll();
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="FREEZE 確定");
for(let i=1;i<=3;i++){
  const kind=await page.locator("#reel"+i+" span:nth-child(2)").getAttribute("data-kind");
  assert.equal(kind,"alt-seven","freeze blue7 reel "+i);
}
console.log("FREEZE_PRESENTATION_PASS");
await page.waitForFunction(()=>!document.querySelector("#lever")?.disabled);

// 2. Rare-role presentation/cut-in.
await reset();
await setFlag("強チェリー");
await page.click("#lever");
await stopAll();
await page.waitForFunction(()=>document.querySelector("#cutinTitle")?.textContent==="強チェリー");
assert(await page.locator("#cutinLayer").evaluate(el=>el.classList.contains("show")),"strong cherry cutin");
console.log("RARE_ROLE_PRESENTATION_PASS");
await page.waitForFunction(()=>!document.querySelector("#lever")?.disabled);

// 3. Direct-hit presentation, then AT internal add-games presentation.
await reset();
await setFlag("中位AT＋ストック直撃");
await page.click("#lever");
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="中位AT＋STOCK");
await page.waitForFunction(()=>document.querySelector("#tier")?.textContent==="中位");
await page.waitForFunction(()=>!document.querySelector("#lever")?.disabled);
console.log("DIRECT_SPECIAL_PRESENTATION_PASS");

await setFlag("AddGames");
await page.click("#lever");
await stopAll();
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="G数上乗せ");
console.log("AT_ADD_GAMES_PRESENTATION_PASS");

// 4. Mode-stage presentation path.
await page.waitForFunction(()=>!document.querySelector("#lever")?.disabled);
await reset();
await setFlag("NormalB");
await page.click("#lever");
await stopAll();
await page.waitForFunction(()=>document.querySelector("#stageScreen")?.dataset.scene==="evening");
console.log("MODE_STAGE_PRESENTATION_PASS");

assert.equal(pageErrors.length,0,pageErrors.join("\n"));
await browser.close();
console.log("V2_PRESENTATION_BROWSER_PASS");
