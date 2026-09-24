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
  if(!(await d.evaluate(el=>el.open)))await page.locator("#v2Debug > summary").click();
  await page.selectOption("#flagPicker",{label});
  await page.click("#applyFlag");
  await page.waitForFunction(()=>document.querySelector("#flagStatus")?.textContent.includes("C++に登録"));
  if(await d.evaluate(el=>el.open))await page.locator("#v2Debug > summary").click();
}
async function stopAll(){
  for(let i=0;i<3;i++){
    await page.waitForFunction(idx=>!document.querySelector('.stop[data-stop="'+idx+'"]')?.disabled,i);
    await page.click('.stop[data-stop="'+i+'"]');
  }
}
async function waitReady(){await page.waitForFunction(()=>!document.querySelector("#lever")?.disabled);}
async function reset(){await page.click("#reset");await waitReady();}
async function playOne(){
  await page.click("#lever");
  await page.waitForFunction(()=>document.querySelector("#debugState")?.textContent.includes('"phase": 1'));
  await stopAll();
  await waitReady();
}

// FREEZE: blackout -> reverse -> premium -> target -> blue777 -> confirmed.
await setFlag("フリーズ");
await page.click("#lever");
await page.waitForTimeout(100);
assert(await page.locator(".machine").evaluate(el=>el.classList.contains("freeze-all-off")));
for(let i=1;i<=3;i++)assert(await page.locator("#reel"+i).evaluate(el=>el.classList.contains("reverse-spinning")));
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="PREMIUM FREEZE");
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="🟦7を狙え");
await stopAll();
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="FREEZE 確定");
for(let i=1;i<=3;i++)assert.equal(await page.locator("#reel"+i+" span:nth-child(2)").getAttribute("data-kind"),"alt-seven");
console.log("FREEZE_PRESENTATION_PASS");
await waitReady();

// Rare-role cue/cutin.
await reset();await setFlag("強チェリー");
await page.click("#lever");await stopAll();
await page.waitForFunction(()=>document.querySelector("#cutinTitle")?.textContent==="強チェリー");
assert(await page.locator("#cutinLayer").evaluate(el=>el.classList.contains("show")));
console.log("RARE_ROLE_PRESENTATION_PASS");
await waitReady();

// Direct hit must present, then go to hidden start-symbol waiting rather than instant AT.
await reset();await setFlag("中位AT＋ストック直撃");
await page.click("#lever");
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="中位AT＋STOCK");
await page.waitForFunction(()=>document.querySelector("#eventNote")?.textContent.includes("開始図柄"));
assert.notEqual(await page.locator("#tier").textContent(),"中位","direct hit must wait for entry symbols");
console.log("DIRECT_HIT_ENTRY_WAIT_PASS");
await waitReady();

// Entry gate uses 1/2 attempts. Play until middle AT actually starts.
let entered=false;
for(let n=0;n<20&&!entered;n++){
  await page.click("#lever");
  await stopAll();
  await waitReady();
  entered=(await page.locator("#tier").textContent())==="中位";
}
assert(entered,"middle AT entry within 20 gate attempts");
console.log("ENTRY_GATE_TO_AT_PASS");

// AT internal add-games presentation.
await setFlag("AddGames");
await page.click("#lever");await stopAll();
await page.waitForFunction(()=>document.querySelector("#cinematicTitle")?.textContent==="G数上乗せ");
console.log("AT_ADD_GAMES_PRESENTATION_PASS");
await waitReady();

// Mode stage mapping.
await reset();await setFlag("NormalB");
await page.click("#lever");await stopAll();
await page.waitForFunction(()=>document.querySelector("#stageScreen")?.dataset.scene==="evening");
console.log("MODE_STAGE_PRESENTATION_PASS");

assert.equal(pageErrors.length,0,pageErrors.join("\n"));
await browser.close();
console.log("V2_PRESENTATION_BROWSER_PASS");
