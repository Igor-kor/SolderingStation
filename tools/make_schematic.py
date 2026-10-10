# -*- coding: utf-8 -*-
"""Генератор принципиальной схемы паяльной станции (фен) на Arduino Uno.

Запуск из корня проекта:  python tools/make_schematic.py
Результат: img/schematic.png

Номиналы соответствуют узловым схемам img/cheme1.jpg, cheme2.jpg,
cheme3.png и подключению из src/Config.h.
"""
import os
import schemdraw
import schemdraw.elements as elm

d = schemdraw.Drawing(show=False)
d.config(unit=2.5, fontsize=11)

GND24 = '0В (24В)'   # земля стороны 24 В (развязана с GND Arduino оптопарой)


def net(xy, text, direction='right', lead=1.8):
    """Короткий стабиль цепи с открытой точкой и подписью."""
    dirs = {'right': elm.Line().right, 'left': elm.Line().left,
            'up': elm.Line().up, 'down': elm.Line().down}
    loc = {'right': 'right', 'left': 'left', 'up': 'right', 'down': 'right'}
    e = dirs[direction](lead).at(xy)
    d.add(e)
    d.add(elm.Dot(open=True).at(e.end).label(text, loc=loc[direction]))


def caption(xy, text, size=10):
    d.add(elm.Label().at(xy).label(text, fontsize=size))


# ============================== Заголовок ==============================
caption((46, 13.5), 'Паяльная станция (фен) на Arduino Uno — принципиальная схема', 15)

# ==================== Силовая часть: ТЭН + симистор + MOC ====================
d += elm.Line().at((0, 10)).to((34, 10)).label('L 220В', loc='left')
d += elm.Line().at((0, 0)).to((31, 0)).label('N 220В', loc='left')

d += elm.Dot().at((30, 10))
heater = elm.Resistor().at((30, 10)).down().label('ТЭН\nфена', loc='bottom')
d += heater
triac = elm.Triac().down().at((30, heater.end[1]))
d += triac
d += elm.Line().at(triac.end).toy(0)
d += elm.Dot().at((30, 0))
caption((33.2, 5.5), 'BT139')

moc = elm.Ic(pins=[elm.IcPin(name='in1', pin='1', side='left', slot='2/2'),
                   elm.IcPin(name='in2', pin='2', side='left', slot='1/2'),
                   elm.IcPin(name='o6', pin='6', side='right', slot='2/2'),
                   elm.IcPin(name='o4', pin='4', side='right', slot='1/2')],
             edgepadW=1.5, pinspacing=2.5, w=4)
d += moc.right().anchor('center').at((21, 5.5)).label('MOC3021', 'top', fontsize=10)

r330moc = elm.Resistor().at(moc.absanchors['in1']).left().label('330', loc='top')
d += r330moc
d += elm.Ground().right().at(moc.absanchors['in2'])

d += elm.Line().at(moc.absanchors['o4']).tox(26.5)
d += elm.Line().toy(triac.gate[1])
d += elm.Line().to(triac.gate)
r1k = elm.Resistor().at(moc.absanchors['o6']).right().label('1к', loc='top')
d += r1k
d += elm.Line().at(r1k.end).toy(8.5)
d += elm.Line().tox(30)
d += elm.Dot().at((30, 8.5))
caption((16, 9.3), 'нагреватель: фазовое управление')

# ==================== Детектор нуля (LTV814) ====================
d += elm.Dot().at((5, 10))
d += elm.Line().at((5, 10)).toy(-3.75)
r33a = elm.Resistor().right().label('33к', loc='top')
d += r33a.at((5, -3.75))
d += elm.Dot().at((10, 0))
r33b = elm.Resistor().at((10, 0)).down().label('33к', loc='bottom')
d += r33b

ltv = elm.Ic(pins=[elm.IcPin(name='in1', pin='1', side='left', slot='2/2'),
                   elm.IcPin(name='in2', pin='2', side='left', slot='1/2'),
                   elm.IcPin(name='oc', pin='4', side='right', slot='2/2'),
                   elm.IcPin(name='oe', pin='3', side='right', slot='1/2')],
             edgepadW=1.5, pinspacing=2.5, w=4)
d += ltv.right().anchor('center').at((15.5, -5)).label('LTV814', 'bottom', fontsize=10)
d += elm.Line().at(r33a.end).tox(ltv.absanchors['in1'][0])
d += elm.Line().to(ltv.absanchors['in1'])
d += elm.Line().at(r33b.end).toy(-6.25)
d += elm.Line().tox(ltv.absanchors['in2'][0])
d += elm.Line().to(ltv.absanchors['in2'])

d += elm.Line().at(ltv.absanchors['oc']).tox(19.5)
d += elm.Dot().at((19.5, -3.75))
r10up = elm.Resistor().at((19.5, -3.75)).up().label('10к', loc='top')
d += r10up
net(r10up.end, '+5В', 'up', 1.0)
d += elm.Ground().right().at(ltv.absanchors['oe'])
caption((9, -8.6), 'детектор нуля → D3 (INT1)')

# ==================== Питание ====================
psu1 = elm.Ic(pins=[elm.IcPin(name='acL', pin='~L', side='left', slot='2/2'),
                    elm.IcPin(name='acN', pin='~N', side='left', slot='1/2'),
                    elm.IcPin(name='p5', pin='+5В', side='right', slot='2/2'),
                    elm.IcPin(name='g1', pin='GND', side='right', slot='1/2')],
              edgepadW=1.2, pinspacing=2.2, w=5)
d += psu1.right().anchor('center').at((5, -13)).label('БП\n220В→5В', 'left', fontsize=10)
psu2 = elm.Ic(pins=[elm.IcPin(name='acL', pin='~L', side='left', slot='2/2'),
                    elm.IcPin(name='acN', pin='~N', side='left', slot='1/2'),
                    elm.IcPin(name='p24', pin='+24В', side='right', slot='2/2'),
                    elm.IcPin(name='g2', pin='0В', side='right', slot='1/2')],
              edgepadW=1.2, pinspacing=2.2, w=5)
d += psu2.right().anchor('center').at((5, -18.5)).label('БП\n220В→24В', 'left', fontsize=10)

d += elm.Dot().at((3, 10))
d += elm.Line().at((3, 10)).toy(psu1.absanchors['acL'][1])
d += elm.Line().to(psu1.absanchors['acL'])
d += elm.Dot().at((1, 0))
d += elm.Line().at((1, 0)).toy(psu1.absanchors['acN'][1])
d += elm.Line().to(psu1.absanchors['acN'])
d += elm.Line().at(psu1.absanchors['acL']).toy(psu2.absanchors['acL'][1])
d += elm.Line().to(psu2.absanchors['acL'])
d += elm.Line().at(psu1.absanchors['acN']).toy(psu2.absanchors['acN'][1])
d += elm.Line().to(psu2.absanchors['acN'])
net(psu1.absanchors['p5'], '+5В')
d += elm.Ground().right().at(psu1.absanchors['g1'])
d += elm.Ground().right().at(psu2.absanchors['g2']).label(GND24, loc='right')

# +24В к секции вентилятора
d += elm.Line().at(psu2.absanchors['p24']).tox(17)
d += elm.Line().toy(-12)
d += elm.Dot().at((17, -12))

# ==================== Вентилятор (24 В) ====================
d += elm.Line().at((15.5, -12)).to((25, -12)).label('+24В', loc='left')
d += elm.Dot().at((17, -12))
motor = elm.Motor().at((21, -12)).down().label('M1\nтурбина', loc='bottom')
d += motor

fet = elm.NFet().at(motor.end).down().label('IRFZ44N', loc='bottom', ofst=0.9)
d += fet
d += elm.Ground().right().at(fet.source).label(GND24, loc='right')

# диод обратного хода параллельно мотору (катодом к +24В)
d += elm.Line().at((25, -12)).down(0.7)
dfly = elm.Diode().at((25, -12.7)).down().label('D', loc='bottom').reverse()
d += dfly
d += elm.Line().at(dfly.end).toy(fet.drain[1])
d += elm.Dot().at((25, fet.drain[1]))
d += elm.Line().at(motor.end).tox(25)
d += elm.Dot().at((21, fet.drain[1]))

# оптопара PC817: вход от D5, эмиттер на затвор
opt = elm.Optocoupler().at((13, -17.3)).right().label('PC817', loc='bottom')
d += opt
rin = elm.Resistor().at(opt.anode).left().label('330', loc='top')
d += rin
net(rin.end, 'D5 (ШИМ)', 'left')
d += elm.Ground().right().at(opt.cathode)
d += elm.Line().at(opt.collector).toy(-12)
d += elm.Dot().at((opt.collector[0], -12))
rgate = elm.Resistor().at(opt.emitter).right().label('10', loc='top')
d += rgate
d += elm.Line().at(rgate.end).tox(fet.gate[0] - 0.9)
d += elm.Dot().at((rgate.end[0], opt.emitter[1]))
rpull = elm.Resistor().at((rgate.end[0], opt.emitter[1])).down().label('10к', loc='bottom')
d += rpull
d += elm.Ground().right().at(rpull.end).label(GND24, loc='right')
d += elm.Line().at(rgate.end).to(fet.gate)
caption((10, -21.3), 'вентилятор (24 В, развязка PC817)')

# ==================== Arduino UNO ====================
left_pins = ['D9', 'D3', 'D5', '5V', 'GND']
right_pins = ['D2', 'D4', 'D8', 'D7', 'D10', 'A4', 'A5', '5V', 'GND']
uno = elm.Ic(pins=[elm.IcPin(name=p, side='left', slot=f'{i + 1}/{len(left_pins)}')
                   for i, p in enumerate(left_pins)]
                  + [elm.IcPin(name=p, side='right', slot=f'{i + 1}/{len(right_pins)}')
                     for i, p in enumerate(right_pins)]
                  + [elm.IcPin(name='A0', side='bottom', slot='1/1')],
             edgepadW=2.5, pinspacing=2.2, w=8)
d += uno.right().anchor('center').at((50, -2)).label('Arduino\nUNO', 'center', fontsize=13)

# D9 -> 330 Ом входа MOC
d += elm.Line().at(r330moc.end).tox(14)
d += elm.Line().toy(uno.absanchors['D9'][1])
d += elm.Line().to(uno.absanchors['D9'])
# D3 <- детектор нуля
d += elm.Line().at((19.5, -3.75)).tox(42)
d += elm.Line().toy(uno.absanchors['D3'][1])
d += elm.Line().to(uno.absanchors['D3'])
# D5 <- вход оптопары вентилятора
d += elm.Line().at(rin.end).toy(-21)
d += elm.Line().tox(44)
d += elm.Line().toy(uno.absanchors['D5'][1])
d += elm.Line().to(uno.absanchors['D5'])
# питание Arduino метками
net(uno.absanchors['5V'], '+5В', 'left', 1.2)
d += elm.Ground().right().at(uno.absanchors['GND'])
net(uno.absanchors['5V'.replace('x', '')] if False else (54.9, -9.5), '+5В', 'right', 1.2)
d += elm.Ground().right().at((54.9, -11.7))

# ==================== Управление ====================
enc = elm.Ic(pins=[elm.IcPin(name='A', side='left', slot='3/4'),
                   elm.IcPin(name='B', side='left', slot='2/4'),
                   elm.IcPin(name='K', side='left', slot='1/4'),
                   elm.IcPin(name='e', pin='GND', side='right', slot='2/2')],
             edgepadW=1.5, pinspacing=2.2, w=5)
d += enc.right().anchor('center').at((80, 6)).label('энкодер\n(кнопка — K)', 'right', fontsize=10)

d += elm.Line().at(enc.absanchors['A']).tox(62)
d += elm.Line().toy(uno.absanchors['D2'][1])
d += elm.Line().to(uno.absanchors['D2'])
d += elm.Line().at(enc.absanchors['B']).tox(64)
d += elm.Line().toy(uno.absanchors['D4'][1])
d += elm.Line().to(uno.absanchors['D4'])
d += elm.Line().at(enc.absanchors['K']).tox(66)
d += elm.Line().toy(uno.absanchors['D8'][1])
d += elm.Line().to(uno.absanchors['D8'])
d += elm.Ground().right().at(enc.absanchors['e'])

# кнопка сохранения (активная низкая, подтяжка 10к)
sw1 = elm.Switch().at((72, -5)).up().label('кнопка\nсохранения', loc='right')
d += sw1
d += elm.Ground().right().at(sw1.start)
d += elm.Dot().at(sw1.end)
rb1 = elm.Resistor().at(sw1.end).left().label('10к', loc='top')
d += rb1
net(rb1.end, '+5В', 'left', 1.0)
d += elm.Line().at(sw1.end).tox(60)
d += elm.Line().toy(uno.absanchors['D7'][1])
d += elm.Line().to(uno.absanchors['D7'])

# геркон подставки
sw2 = elm.Switch().at((76, -10.5)).up().label('геркон\n(подставка)', loc='right')
d += sw2
d += elm.Ground().right().at(sw2.start)
d += elm.Dot().at(sw2.end)
rb2 = elm.Resistor().at(sw2.end).right().label('10к', loc='top')
d += rb2
net(rb2.end, '+5В', 'right', 1.0)
d += elm.Line().at(sw2.end).tox(58)
d += elm.Line().toy(uno.absanchors['D10'][1])
d += elm.Line().to(uno.absanchors['D10'])
caption((80, 1.2), 'управление (активные низкие)')

# ==================== Дисплей ====================
oled = elm.Ic(pins=[elm.IcPin(name='vcc', pin='VCC', side='left', slot='4/4'),
                    elm.IcPin(name='gnd', pin='GND', side='left', slot='3/4'),
                    elm.IcPin(name='scl', pin='SCL', side='left', slot='2/4'),
                    elm.IcPin(name='sda', pin='SDA', side='left', slot='1/4')],
              edgepadW=1.5, pinspacing=2.2, w=6)
d += oled.right().anchor('center').at((82, -16)).label('OLED\nSSD1306\n128×32', 'right', fontsize=10)
net(oled.absanchors['vcc'], '+5В', 'left', 1.2)
d += elm.Ground().right().at(oled.absanchors['gnd'])
d += elm.Line().at(oled.absanchors['scl']).tox(70)
d += elm.Line().toy(uno.absanchors['A5'][1])
d += elm.Line().to(uno.absanchors['A5'])
d += elm.Line().at(oled.absanchors['sda']).tox(72)
d += elm.Line().toy(uno.absanchors['A4'][1])
d += elm.Line().to(uno.absanchors['A4'])

# ==================== Термопара → A0 ====================
d += elm.Dot(open=True).at((60, -22.5)).label('ТК\n(K-тип)', loc='left')
r4 = elm.Resistor().at((60, -22.5)).right().label('R4 10к', loc='top')
d += r4
op = elm.Opamp().at((69, -22.5)).right().label('LM358', loc='bottom', fontsize=10)
d += op
d += elm.Line().at(r4.end).toy(op.in2[1])
d += elm.Line().to(op.in2)
# R3: инвертирующий вход на землю
d += elm.Line().at(op.in1).tox(64)
r3 = elm.Resistor().at((64, op.in1[1])).down().label('R3 1к', loc='bottom')
d += r3
d += elm.Ground().right().at(r3.end)
# обратная связь R5
d += elm.Line().at(op.out).toy(-18.5)
r5 = elm.Resistor().at((op.out[0], -18.5)).left().label('R5 5,1к', loc='top')
d += r5
d += elm.Line().at(r5.end).tox(op.in1[0] - 2)
d += elm.Line().toy(op.in1[1])
d += elm.Line().to(op.in1)
d += elm.Dot().at((op.in1[0] - 2, op.in1[1]))
# смещение R1 120к от +5В к выходу
d += elm.Line().at(op.out).tox(78)
d += elm.Dot().at((78, op.out[1]))
r1 = elm.Resistor().at((78, op.out[1])).up().label('R1 120к', loc='top')
d += r1
net(r1.end, '+5В', 'up', 1.0)
# фильтр R2-C1 и выход на A0
r2 = elm.Resistor().at((78, op.out[1])).right().label('R2 1М', loc='top')
d += r2
d += elm.Dot().at(r2.end)
c1 = elm.Capacitor().at(r2.end).down().label('C1\n0,01мкФ', loc='bottom')
d += c1
d += elm.Ground().right().at(c1.end)
d += elm.Line().at(r2.end).toy(-27)
d += elm.Line().tox(uno.absanchors['A0'][0])
d += elm.Line().to(uno.absanchors['A0'])
caption((62, -20.2), 'усилитель термопары (К≈6,1) → A0')
caption((78, -25.2), 'LM358: п.8 = +5В, п.4 = GND')

out = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'img', 'schematic')
d.save(out + '.png', dpi=160)
d.save(out + '.svg')
print('saved:', out + '.png / .svg')
