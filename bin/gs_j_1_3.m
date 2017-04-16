%% Gauss-Seidel
clf; close all;
load('gs1.txt')
load('gs2.txt')
load('gs3.txt')

itsPlot = 1000;

iter = gs1(1:itsPlot, 1);
avgGS = (gs1(1:itsPlot, 2) + gs2(1:itsPlot, 2) + gs3(1:itsPlot, 2)) / 3;

figure(1)
plot(gs1(1:itsPlot, 1), gs1(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel 1', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 5500])

figure(2)
plot(gs2(1:itsPlot, 1), gs2(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel 2', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 5500])

figure(3)
plot(gs3(1:itsPlot, 1), gs3(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel 3', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 5500])

figure(4)
plot(gs1(1:itsPlot, 1), avgGS(1:itsPlot))
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel, 3 times average', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 5500])

%% Jacobi
clf; close all;
load('j1.txt')
load('j2.txt')
load('j3.txt')

iter = j1(1:itsPlot, 1);
avgJ = (j1(1:itsPlot, 2) + j2(1:itsPlot, 2) + j3(1:itsPlot, 2)) / 3;

figure(1)
plot(j1(1:itsPlot, 1), j1(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Jacobi 1', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 5500])

figure(2)
plot(j2(1:itsPlot, 1), j2(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Jacobi 2', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 5500])

figure(3)
plot(j3(1:itsPlot, 1), j3(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Jacobi 3', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 5500])

figure(4)
plot(j1(1:itsPlot, 1), avgJ(1:itsPlot))
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Jacobi, 3 times average', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 5500])

%% GS v Jacobi
% !Requires both sections above to be run first
clf; close all;

hold on
plot(j1(1:itsPlot, 1), avgJ)
plot(gs1(1:itsPlot, 1), avgGS)

xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel v Jacobi, 3 times average', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 5500])

xvertsJ = [iter(1:end-1); flipud(iter(2:end)); iter(1)];
yvertsJ = [zeros(999, 1); flipud(avgJ(2:end)); avgJ(1)];
pJ = patch(xvertsJ, yvertsJ, 'b', 'FaceAlpha', 0.6, 'LineStyle', 'none');

xvertsGS = [iter(1:end-1); flipud(iter(2:end)); iter(1)];
yvertsGS = [zeros(999, 1); flipud(avgGS(2:end)); avgGS(1)];
pGS = patch(xvertsGS, yvertsGS, 'r', 'FaceAlpha', 0.6, 'LineStyle', 'none');

cTotJ = trapz(avgJ)
cTotGS = trapz(avgGS)

relC = cTotJ/cTotGS

dim = [0.55 0.4 0.5 0.3];
str = sprintf('GS: int(sum(c)) = %2.3d\nJ: int(sum(c)) = %2.3d\nJ/GS = %1.3f', cTotGS, cTotJ, relC);
annotation('textbox', dim, 'String', str, 'FitBoxToText', 'on', 'Interpreter', 'LaTeX')

legend({'Jacobi', 'Gauss-Seidel', 'Jacobi', 'Gauss-Seidel'}, 'Interpreter', 'LaTeX')
